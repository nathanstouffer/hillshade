#include "hillshade/application.h"

#include <chrono>
#include <filesystem>
#include <set>

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

    static const char* c_shader_dir = "shaders";
    static const char* c_tiff_dir = "tiff";
    static const char* c_terrarium_dir = "terrarium";

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
        m_imgui_impl = std::make_unique<Diligent::ImGuiImplWin32>(Diligent::ImGuiDiligentCreateInfo(&*m_device, desc), hWnd);
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { static_cast<float>(m_width), static_cast<float>(m_height) };

        create_resources();

        std::string path = (*(++std::filesystem::directory_iterator(c_tiff_dir))).path().string();
        load_dem(path);

        return true;
    }

    void application::update()
    {
        if (m_render_ui) { render_ui(); }
    }

    void application::render_ui()
    {
        m_imgui_impl->NewFrame(m_width, m_height, Diligent::SURFACE_TRANSFORM_IDENTITY);
        
        // debug window
        {
            ImGui::BeginMainMenuBar();

            if (ImGui::BeginMenu("tiff"))
            {
                for (std::filesystem::directory_entry const& file : std::filesystem::directory_iterator(c_tiff_dir))
                {
                    std::string path = file.path().string();
                    bool selected = m_dem_path == path;
                    if (ImGui::MenuItem(file.path().stem().generic_string().c_str(), nullptr, selected, !selected))
                    {
                        load_dem(path);
                    }
                }
                ImGui::EndMenu();
            }

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
                ImGui::DragFloat("delta scalar", &m_delta_scalar, 0.0001f, 0.f, 0.01f, "%.4f");
            }
            ImGui::Separator();
            // info block
            {
                ImGui::Text("Info");
                ImGui::Text("Eye: (%.1f, %.1f, %.1f)", m_camera.eye.x, m_camera.eye.y, m_camera.eye.z);
                ImGui::Text("Theta: %.1f  Phi: %.1f", stf::math::to_degrees(m_camera.theta), stf::math::to_degrees(m_camera.phi));

                stff::vec3 light_dir = light_direction(m_azimuth, m_altitude);
                ImGui::Text("Light direction: (%.3f, %.3f, %.3f)", light_dir.x, light_dir.y, light_dir.z);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::End();
        }
    }

    void application::render()
    {
        update();

        // set render targets before issuing any draw command.
        // note that present() unbinds the back buffer if it is set as render target.
        auto* rtv = m_swap_chain->GetCurrentBackBufferRTV();
        auto* dsv = m_swap_chain->GetDepthBufferDSV();
        m_immediate_context->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // clear back buffer
        m_immediate_context->ClearRenderTarget(rtv, reinterpret_cast<float*>(&m_clear_color), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_immediate_context->ClearDepthStencil(dsv, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Diligent::MapHelper<constants> consts(m_immediate_context, m_shader_constants, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
        
        m_camera.aspect = aspect_ratio();
        consts->view_proj = m_camera.perspective() * m_camera.view();

        if (m_terrain)
        {
            stff::aabb2 bounds = m_terrain->bounds().as<float>();
            consts->bounds = stff::vec4(bounds.min, bounds.max);
            stff::vec2 res = stff::vec2(static_cast<float>(m_terrain->width()), static_cast<float>(m_terrain->height()));
            consts->resolution = stff::vec4(res, 1.0f / res);
        }
        
        consts->albedo = m_albedo.as_vec();
        consts->light_dir = light_direction(m_azimuth, m_altitude);
        consts->ambient_intensity = m_ambient_intensity;
        
        consts->eye = m_camera.eye;
        consts->exaggeration = m_exaggeration;

        // set the pipeline state in the immediate context
        m_immediate_context->SetPipelineState(m_pso);
        m_immediate_context->CommitShaderResources(m_srb, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Diligent::DrawAttribs draw_attrs;
        draw_attrs.NumVertices = 6;
        m_immediate_context->Draw(draw_attrs);

        if (m_render_ui) { m_imgui_impl->Render(m_immediate_context); }
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

    void application::create_resources()
    {
        Diligent::GraphicsPipelineStateCreateInfo pso_info;

        pso_info.PSODesc.Name = "Simple quad PSO";
        pso_info.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        pso_info.GraphicsPipeline.NumRenderTargets = 1;
        pso_info.GraphicsPipeline.RTVFormats[0] = m_swap_chain->GetDesc().ColorBufferFormat;
        pso_info.GraphicsPipeline.DSVFormat = m_swap_chain->GetDesc().DepthBufferFormat;
        pso_info.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pso_info.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        pso_info.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::False;

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
            shader_info.Desc.Name = "quad vertex shader";
            shader_info.FilePath = path.c_str();
            m_device->CreateShader(shader_info, &vertex_shader);
        }

        // create a pixel shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pixel_shader;
        {
            std::string path = std::string(c_shader_dir) + "/hillshade.psh";
            shader_info.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            shader_info.EntryPoint = "main";
            shader_info.Desc.Name = "quad pixel shader";
            shader_info.FilePath = path.c_str();
            m_device->CreateShader(shader_info, &pixel_shader);
        }

        // create the pipeline state
        pso_info.pVS = vertex_shader;
        pso_info.pPS = pixel_shader;

        pso_info.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        // shader variables should typically be mutable, which means they are expected to change on a per-instance basis
        Diligent::ShaderResourceVariableDesc vars[] =
        {
            { Diligent::SHADER_TYPE_PIXEL, "g_terrain", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE }
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
            {Diligent::SHADER_TYPE_PIXEL, "g_terrain", desc}
        };
        pso_info.PSODesc.ResourceLayout.ImmutableSamplers = immutable_samplers;
        pso_info.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(immutable_samplers);

        m_device->CreateGraphicsPipelineState(pso_info, &m_pso);

        m_pso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "VSConstants")->Set(m_shader_constants);
        m_pso->GetStaticVariableByName(Diligent::SHADER_TYPE_PIXEL,  "PSConstants")->Set(m_shader_constants);

        m_pso->CreateShaderResourceBinding(&m_srb, true);
    }

    void application::load_dem(std::string const& path)
    {
        m_dem_path = path;
        m_terrain = std::make_unique<terrain>(m_dem_path);

        // compute camera information
        {
            float z = std::max(m_terrain->range().b, m_terrain->bounds().as<float>().diagonal().length());
            stff::vec3 eye(0, 0, z);
            m_camera = stff::scamera(eye, stff::constants::half_pi, stff::constants::pi, 0.1f, 10000.f, aspect_ratio(), stff::scamera::c_default_fov);
        }

        // load gpu resources
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
            m_srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "g_terrain")->Set(m_texture_srv);
        }
    }

}