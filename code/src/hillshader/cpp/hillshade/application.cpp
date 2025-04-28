#include "hillshade/application.hpp"

#include <chrono>
#include <fstream>
#include <filesystem>
#include <set>

#include <nlohmann/json.hpp>

#include <stf/alg/intersect.hpp>

#include <Common/interface/DataBlobImpl.hpp>
#include <Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>
#include <Graphics/GraphicsTools/interface/GraphicsUtilities.h>
#include <Graphics/GraphicsTools/interface/MapHelper.hpp>
#include <TextureLoader/interface/TextureLoader.h>
#include <TextureLoader/interface/TextureUtilities.h>
#include <imgui.h>

namespace
{

    stff::vec3 light_direction(float const azimuth, float const altitude)
    {
        // transform azimuth and altitude into spherical coordinates
        float const theta = stff::constants::half_pi - stf::math::to_radians(azimuth);
        float const phi = stff::constants::half_pi - stf::math::to_radians(altitude);
        return -stf::math::unit_vector(theta, phi);
    }

}

namespace hillshade
{

    static constexpr char const* c_start_up_file = "startup.json";
    static constexpr char const* c_shader_dir = "shaders";
    static constexpr char const* c_terrarium_dir = "terrarium";

    static constexpr double c_min_meters_per_quad = 5.0;

    static constexpr float c_min_terrain_offset = 0.5;

    static constexpr float c_wheel_scalar = 1.f / (12.5f * 120.f);
    static constexpr float c_pan_scalar = 0.0008125f;

    struct constants
    {
        stff::mtx4 view_proj;

        stff::vec4 bounds;
        stff::vec4 resolution;
        stff::vec4 albedo;

        stff::vec3 light_dir;
        float ambient_intensity;

        stff::vec3 eye;
        float exaggeration;

        float step_scalar;
        bool flag_3d;
    };

    application::application() {}

    application::~application()
    {
        m_immediate_context->Flush();
    }

    bool application::initialize(HWND hWnd)
    {
        Diligent::SwapChainDesc desc;
#    if EXPLICITLY_LOAD_ENGINE_GL_DLL
        // load the dll and import GetEngineFactoryOpenGL() function
        auto engine = Diligent::LoadGraphicsEngineOpenGL();
#    endif
        auto* factory = engine();
        m_engine_factory = factory;

        Diligent::EngineGLCreateInfo info;
        info.Window.hWnd = hWnd;

        factory->CreateDeviceAndSwapChainGL(info, &m_device, &m_immediate_context, desc, &m_swap_chain);

        // set up imgui
        m_imgui_impl = std::make_unique<Diligent::ImGuiImplWin32>(Diligent::ImGuiDiligentCreateInfo(m_device.RawPtr(), desc), hWnd);
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { static_cast<float>(m_width), static_cast<float>(m_height) };

        create_resources();

        std::ifstream start_up_stream(c_start_up_file);
        if (start_up_stream)
        {
            nlohmann::json json = nlohmann::json::parse(start_up_stream);
            if (json.contains("dem_path"))
            {
                std::string dem_path = json["dem_path"];
                if (std::filesystem::exists(dem_path))
                {
                    load_dem(dem_path);
                }
            }
        }

        return true;
    }

    void application::update()
    {
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            if (m_update_focus)
            {
                std::optional<stff::vec3> opt = cursor_world_pos();
                m_focus = (opt) ? *opt : stff::vec3();
                m_update_focus = false;
            }

            if (ImGui::GetIO().MouseWheel != stff::constants::zero)
            {
                float dist = stf::math::dist(m_camera.eye, m_focus);
                float log_dist = std::log2(dist);
                log_dist -= c_wheel_scalar * ImGui::GetIO().MouseWheel;
                dist = std::pow(2.f, log_dist);
                stff::vec3 dir = (m_camera.eye - m_focus).normalize();
                m_camera.eye = m_focus + dist * dir;
            }

            if (ImGui::GetIO().MouseDown[0])
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                float scalar = c_pan_scalar * m_camera.eye.z;
                m_camera.eye -= scalar * delta.x * m_camera.right();
                m_camera.eye += scalar * delta.y * stf::math::cross(stff::vec3(0, 0, 1), m_camera.right());
            }

            if (ImGui::GetIO().MouseDown[1])
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                ImVec2 size = ImGui::GetIO().DisplaySize;
                float delta_theta = -delta.x / size.x * stff::constants::pi;
                float delta_phi = delta.y / size.y * stff::constants::half_pi;
                m_camera = stf::cam::orbit(m_camera, m_focus, delta_phi, delta_theta);
            }
        }

        // very rudimentary terrain collision
        {
            if (m_terrain && m_flag_3d)
            {
                if (m_terrain->bounds().contains(m_camera.eye.xy.as<double>()))
                {
                    m_camera.eye.z = std::max(m_terrain->sample(m_camera.eye.xy) + c_min_terrain_offset, m_camera.eye.z);
                }
                else
                {
                    m_camera.eye.z = std::max(c_min_terrain_offset, m_camera.eye.z);
                }
            }
            else
            {
                m_camera.eye.z = std::max(c_min_terrain_offset, m_camera.eye.z);
            }
        }
    }

    void application::store_start_up_state()
    {
        std::ofstream out(c_start_up_file);
        out << std::setw(4) << m_start_up_state << std::endl;
    }

    void application::render_ui()
    {
        // debug window
        {
            ImGui::BeginMainMenuBar();

            if (ImGui::BeginMenu("terrarium"))
            {
                for (std::filesystem::directory_entry const& file : std::filesystem::directory_iterator(c_terrarium_dir))
                {
                    if (file.path().extension() != ".json")
                    {
                        std::string path = file.path().string();
                        bool selected = m_dem_path == path;
                        if (ImGui::MenuItem(file.path().stem().generic_string().c_str(), nullptr, selected, !selected))
                        {
                            load_dem(path);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();

            ImGui::Begin("Debugging");


            // configuration block
            {
                ImGui::Text("Configuration");
                ImGui::ColorEdit3("background", reinterpret_cast<float*>(&m_clear_color));
                ImGui::ColorEdit3("albedo", reinterpret_cast<float*>(&m_albedo));
                ImGui::DragFloat("azimuth", &m_azimuth, 0.5f, 0.f, 360.f, "%.1f");
                ImGui::DragFloat("altitude", &m_altitude, 0.5f, 0.f, 90.f, "%.1f");
                ImGui::DragFloat("ambient", &m_ambient_intensity, 0.01f, 0.f, 1.f, "%.2f");
                ImGui::DragFloat("exaggeration", &m_exaggeration, 0.01f, 0.f, 10.f, "%.2f");
                ImGui::DragFloat("step scalar", &m_step_scalar, 0.0001f, 0.f, 0.01f, "%.4f");
                ImGui::Checkbox("render in 3d", &m_flag_3d);
            }
            ImGui::Separator();
            // info block
            {
                ImGui::Text("Info");
                stfd::aabb2 const bounds = (m_terrain) ? m_terrain->bounds() : stfd::aabb2(stfd::vec2(), stfd::vec2());
                ImGui::Text("DEM Bounds: (%.1f, %.1f) - (%.1f, %.1f)", bounds.min.x, bounds.min.y, bounds.max.x, bounds.max.y);
                ImGui::Text("Eye: (%.1f, %.1f, %.1f)", m_camera.eye.x, m_camera.eye.y, m_camera.eye.z);
                ImGui::Text("Theta: %.1f  Phi: %.1f", stf::math::to_degrees(m_camera.theta), stf::math::to_degrees(m_camera.phi));

                ImVec2 mouse_pos = ImGui::GetIO().MousePos;
                ImGui::Text("Mouse Pos (screen): (%.3f, %.3f)", mouse_pos.x, mouse_pos.y);
                std::optional<stff::vec3> opt = cursor_world_pos();
                if (opt)
                {
                    stff::vec3 const& world_pos = *opt;
                    ImGui::Text("Mouse Pos (world): (%.3f, %.3f, %.3f)", world_pos.x, world_pos.y, world_pos.z);
                }

                stff::vec3 light_dir = light_direction(m_azimuth, m_altitude);
                ImGui::Text("Light Direction: (%.3f, %.3f, %.3f)", light_dir.x, light_dir.y, light_dir.z);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::End();
        }
    }

    void application::render()
    {
        // start the ImGui frame (even if we're not going to render it) so that input is refreshed
        m_imgui_impl->NewFrame(m_width, m_height, Diligent::SURFACE_TRANSFORM_IDENTITY);

        update();

        // set render targets before issuing any draw command.
        // note that present() unbinds the back buffer if it is set as render target.
        auto* rtv = m_swap_chain->GetCurrentBackBufferRTV();
        auto* dsv = m_swap_chain->GetDepthBufferDSV();
        m_immediate_context->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // clear back buffer
        m_immediate_context->ClearRenderTarget(rtv, reinterpret_cast<float*>(&m_clear_color), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_immediate_context->ClearDepthStencil(dsv, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        if (m_terrain)
        {
            Diligent::MapHelper<constants> consts(m_immediate_context, m_shader_constants, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);

            m_camera.aspect = aspect_ratio();
            consts->view_proj = m_camera.perspective() * m_camera.view();

            stff::aabb2 bounds = m_terrain->bounds().as<float>();
            consts->bounds = stff::vec4(bounds.min, bounds.max);
            stff::vec2 res = stff::vec2(static_cast<float>(m_terrain->width()), static_cast<float>(m_terrain->height()));
            consts->resolution = stff::vec4(res, 1.0f / res);

            consts->albedo = m_albedo.as_vec();

            consts->light_dir = light_direction(m_azimuth, m_altitude);
            consts->ambient_intensity = m_ambient_intensity;

            consts->eye = m_camera.eye;
            consts->exaggeration = m_exaggeration;

            consts->step_scalar = m_step_scalar;
            consts->flag_3d = m_flag_3d;

            uint64_t const offset = 0;
            Diligent::IBuffer* buffers[] = { m_vertex_buffer };
            m_immediate_context->SetVertexBuffers(0, 1, buffers, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
            m_immediate_context->SetIndexBuffer(m_index_buffer, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            // set the pipeline state in the immediate context
            m_immediate_context->SetPipelineState(m_pso);
            m_immediate_context->CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            Diligent::DrawIndexedAttribs draw_attrs;
            draw_attrs.IndexType = Diligent::VT_UINT32;
            draw_attrs.NumIndices = static_cast<uint32_t>(m_indices.size());
            draw_attrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
            m_immediate_context->DrawIndexed(draw_attrs);
        }

        if (m_render_ui) { render_ui(); }

        m_imgui_impl->Render(m_immediate_context);
    }

    void application::present()
    {
        m_swap_chain->Present();
    }

    void application::resize(Diligent::Uint32 width, Diligent::Uint32 height)
    {
        m_width = width;
        m_height = height;
        if (m_swap_chain)
        {
            m_swap_chain->Resize(width, height);
        }
    }

    void application::reset_camera()
    {
        if (m_terrain)
        {
            float z = std::max(m_terrain->range().b, m_terrain->bounds().as<float>().diagonal().length());
            stff::vec3 eye(0, 0, z);
            if (m_flag_3d)
            {
                eye.z += m_terrain->sample(eye.xy);
            }
            m_camera = stff::scamera(eye, stff::constants::half_pi, stff::constants::pi, 0.1f, 100000.f, aspect_ratio(), stff::scamera::c_default_fov);
        }
        else
        {
            m_camera = stff::scamera(stff::vec3(0, 0, 3000), stff::constants::half_pi, stff::constants::pi, 0.1f, 100000.f, aspect_ratio(), stff::scamera::c_default_fov);
        }
    }

    void application::create_resources()
    {
        Diligent::GraphicsPipelineStateCreateInfo pso_info;

        pso_info.PSODesc.Name = "Simple quad PSO";
        pso_info.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        pso_info.GraphicsPipeline.NumRenderTargets = 1;
        pso_info.GraphicsPipeline.RTVFormats[0] = m_swap_chain->GetDesc().ColorBufferFormat;
        pso_info.GraphicsPipeline.DSVFormat = m_swap_chain->GetDesc().DepthBufferFormat;
        pso_info.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        pso_info.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        pso_info.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::True;

        // create dynamic uniform buffer that will store shader constants
        Diligent::CreateUniformBuffer(m_device, sizeof(constants), "Global shader constants CB", &m_shader_constants);

        Diligent::ShaderCreateInfo shader_info;
        shader_info.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        shader_info.Desc.UseCombinedTextureSamplers = true;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> shader_source_factory;
        m_engine_factory->CreateDefaultShaderSourceStreamFactory(nullptr, &shader_source_factory);
        shader_info.pShaderSourceStreamFactory = shader_source_factory;

        // create a vertex shader
        Diligent::RefCntAutoPtr<Diligent::IShader> vertex_shader;
        {
            std::string path = std::string(c_shader_dir) + "/hillshade.vsh";
            shader_info.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            shader_info.EntryPoint = "main";
            shader_info.Desc.Name = "terrain vertex shader";
            shader_info.FilePath = path.c_str();
            m_device->CreateShader(shader_info, &vertex_shader);
        }

        // create a pixel shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pixel_shader;
        {
            std::string path = std::string(c_shader_dir) + "/hillshade.psh";
            shader_info.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            shader_info.EntryPoint = "main";
            shader_info.Desc.Name = "terrain pixel shader";
            shader_info.FilePath = path.c_str();
            m_device->CreateShader(shader_info, &pixel_shader);
        }

        // create the pipeline state
        pso_info.pVS = vertex_shader;
        pso_info.pPS = pixel_shader;

        Diligent::LayoutElement layout_elems[] =
        {
            // Attribute 0 - vertex position
            Diligent::LayoutElement{0, 0, 2, Diligent::VT_FLOAT32, Diligent::False},
            // Attribute 1 - texture coordinates
            Diligent::LayoutElement{1, 0, 2, Diligent::VT_FLOAT32, Diligent::False}
        };

        pso_info.GraphicsPipeline.InputLayout.LayoutElements = layout_elems;
        pso_info.GraphicsPipeline.InputLayout.NumElements = _countof(layout_elems);

        pso_info.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        // shader variables should typically be mutable, which means they are expected to change on a per-instance basis
        Diligent::ShaderResourceVariableDesc vars[] =
        {
            { Diligent::SHADER_TYPE_VERTEX | Diligent::SHADER_TYPE_PIXEL, "g_terrain", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
        };
        pso_info.PSODesc.ResourceLayout.Variables = vars;
        pso_info.PSODesc.ResourceLayout.NumVariables = _countof(vars);

        // define immutable sampler for g_terrain. Immutable samplers should be used whenever possible
        Diligent::SamplerDesc desc
        {
            Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR, Diligent::FILTER_TYPE_LINEAR,
            Diligent::TEXTURE_ADDRESS_CLAMP, Diligent::TEXTURE_ADDRESS_CLAMP, Diligent::TEXTURE_ADDRESS_CLAMP
        };
        Diligent::ImmutableSamplerDesc immutable_samplers[] =
        {
            { Diligent::SHADER_TYPE_VERTEX | Diligent::SHADER_TYPE_PIXEL, "g_terrain", desc },
        };
        pso_info.PSODesc.ResourceLayout.ImmutableSamplers = immutable_samplers;
        pso_info.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(immutable_samplers);

        m_device->CreateGraphicsPipelineState(pso_info, &m_pso);

        m_pso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "VSConstants")->Set(m_shader_constants);
        m_pso->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL , "PSConstants")->Set(m_shader_constants);

        m_pso->CreateShaderResourceBinding(&m_srb, true);
    }

    std::optional<stff::vec3> application::cursor_world_pos() const
    {
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        ImVec2 res = ImGui::GetIO().DisplaySize;
        stff::vec2 uv = stff::vec2(mouse_pos.x / res.x, mouse_pos.y / res.y);
        stff::ray3 ray = m_camera.ray(uv);

        if (m_terrain && m_flag_3d)
        {
            std::optional<stff::vec3> focus = m_terrain->intersect(ray);
            if (focus.has_value())
            {
                return focus;
            }
        }
        // fall-through case
        stff::plane plane = stff::plane(stff::vec3(), stff::vec3(0, 0, 1));
        return stf::alg::intersect(ray, plane);
    }

    void application::load_dem(std::string const& path)
    {
        release_dem_resources();

        m_dem_path = path;
        m_terrain = std::make_unique<terrain>(m_dem_path);

        reset_camera();

        // compute and load vertex/index buffer
        {
            // compute resolution
            stfd::vec2 const& diagonal = m_terrain->bounds().diagonal();
            double meters_per_pixel = std::max(diagonal.x / m_terrain->width(), diagonal.y / m_terrain->height());
            double meters_per_quad = std::max(meters_per_pixel, c_min_meters_per_quad);
            size_t threshold = static_cast<size_t>(std::min(diagonal.x / meters_per_quad, diagonal.y / meters_per_quad));
            size_t resolution = std::min(threshold, std::max(m_terrain->width(), m_terrain->height()));

            // compute and load vertex buffer
            {
                m_vertices = mesh::vertices(resolution);

                Diligent::BufferDesc desc;
                desc.Name = "Terrain vertex buffer";
                desc.Usage = Diligent::USAGE_IMMUTABLE;
                desc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
                desc.Size = sizeof(mesh::vertex_t) * m_vertices.size();

                Diligent::BufferData data;
                data.pData = m_vertices.data();
                data.DataSize = sizeof(mesh::vertex_t) * m_vertices.size();
                m_device->CreateBuffer(desc, &data, &m_vertex_buffer);
            }

            // compute and load index buffer
            {
                m_indices = mesh::index_strip(resolution);

                Diligent::BufferDesc desc;
                desc.Name = "Terrain index buffer";
                desc.Usage = Diligent::USAGE_IMMUTABLE;
                desc.BindFlags = Diligent::BIND_INDEX_BUFFER;
                desc.Size = sizeof(uint32_t) * m_indices.size();

                Diligent::BufferData data;
                data.pData = m_indices.data();
                data.DataSize = sizeof(uint32_t) * m_indices.size();
                m_device->CreateBuffer(desc, &data, &m_index_buffer);
            }
        }

        // load terrain texture
        {
            Diligent::RefCntAutoPtr<Diligent::IDataBlob> blob = Diligent::DataBlobImpl::Create(sizeof(float) * m_terrain->width() * m_terrain->height(), m_terrain->values().data());

            Diligent::RefCntAutoPtr<Diligent::Image> img;
            Diligent::ImageDesc desc;
            desc.Width = static_cast<Diligent::Uint32>(m_terrain->width());
            desc.Height = static_cast<Diligent::Uint32>(m_terrain->height());
            desc.NumComponents = 1;
            desc.ComponentType = Diligent::VALUE_TYPE::VT_FLOAT32;
            desc.RowStride = sizeof(float) * desc.Width;
            Diligent::Image::CreateFromMemory(desc, blob, &img);

            Diligent::RefCntAutoPtr<Diligent::ITextureLoader> loader;
            Diligent::TextureLoadInfo info;
            info.Format = Diligent::TEXTURE_FORMAT::TEX_FORMAT_R32_FLOAT;
            info.MipLevels = 1;
            info.GenerateMips = false;
            Diligent::CreateTextureLoaderFromImage(img, info, &loader);

            loader->CreateTexture(m_device, &m_texture);

            m_texture_srv = m_texture->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);
            m_srb->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, "g_terrain")->Set(m_texture_srv, Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
            m_srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL , "g_terrain")->Set(m_texture_srv, Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
        }

        m_start_up_state["dem_path"] = path;
        store_start_up_state();
    }

    void application::release_dem_resources()
    {
        if (m_texture) { m_texture = nullptr; }
        if (m_texture_srv) { m_texture_srv = nullptr; }
        if (m_vertex_buffer) { m_vertex_buffer = nullptr; }
        if (m_index_buffer) { m_index_buffer = nullptr; }
    }

}