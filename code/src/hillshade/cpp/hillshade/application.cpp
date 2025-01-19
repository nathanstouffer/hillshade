#include "hillshade/application.h"

#include <filesystem>

#include <Common/interface/DataBlobImpl.hpp>
#include <Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>
#include <TextureLoader/interface/TextureLoader.h>
#include <TextureLoader/interface/TextureUtilities.h>
#include <imgui.h>

namespace hillshade
{

    static const char* c_tiffs_dir = "tiffs";

    static const char* s_vertex_shader_source =
    R"(
        struct PSInput 
        { 
            float4 pos  : SV_POSITION; 
            float2 uv   : TEX_COORD;
        };

        void main(in uint vertex_id : SV_VertexID, out PSInput pixel_input) 
        {
            float4 positions[6];
            positions[0] = float4(-0.5, -0.5, 0.0, 1.0);
            positions[1] = float4(-0.5, +0.5, 0.0, 1.0);
            positions[2] = float4(+0.5, +0.5, 0.0, 1.0);
            positions[3] = float4(+0.5, +0.5, 0.0, 1.0);
            positions[4] = float4(+0.5, -0.5, 0.0, 1.0);
            positions[5] = float4(-0.5, -0.5, 0.0, 1.0);

            float3 colors[6];
            colors[0] = float3(1.0, 0.0, 0.0);
            colors[1] = float3(1.0, 1.0, 0.0);
            colors[2] = float3(0.0, 1.0, 0.0);
            colors[3] = float3(0.0, 1.0, 0.0);
            colors[4] = float3(1.0, 1.0, 0.0);
            colors[5] = float3(1.0, 0.0, 0.0);

            pixel_input.pos = positions[vertex_id];
            pixel_input.uv  = positions[vertex_id].xy + float2(0.5); // colors[vertex_id];
        }
    )";

    static const char* s_pixel_shader_source =
    R"(
        Texture2D       g_terrain;
        SamplerState    g_terrain_sampler;

        struct PSInput
        { 
            float4 pos  : SV_POSITION; 
            float2 uv   : TEX_COORD; 
        };

        struct PSOutput
        { 
            float4 color : SV_TARGET; 
        };

        void main(in PSInput pixel_input, out PSOutput pixel_output)
        {
            float4 color = g_terrain.Sample(g_terrain_sampler, pixel_input.uv);
            pixel_output.color = float4(float3(color.r), 1.0);
        }
    )";

    application::application()
    {

    }

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

        Diligent::EngineGLCreateInfo info;
        info.Window.hWnd = hWnd;

        factory->CreateDeviceAndSwapChainGL(info, &m_device, &m_immediate_context, desc, &m_swap_chain);

        // set up imgui
        m_imgui_impl = std::make_unique<Diligent::ImGuiImplWin32>(Diligent::ImGuiDiligentCreateInfo(&*m_device, desc), hWnd);
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { static_cast<float>(m_width), static_cast<float>(m_height) };

        create_resources();

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

            if (ImGui::BeginMenu("tiffs"))
            {
                for (std::filesystem::directory_entry const& file : std::filesystem::directory_iterator(c_tiffs_dir))
                {
                    std::string name = file.path().filename().string();
                    bool selected = m_tiff_name == name;
                    if (ImGui::MenuItem(name.c_str(), nullptr, selected, !selected))
                    {
                        load_tiff(name);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();

            ImGui::Begin("Debugging"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::ColorEdit3("background", reinterpret_cast<float*>(&m_clear_color));
            ImGui::ColorEdit3("albedo", reinterpret_cast<float*>(&m_albedo));

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

        Diligent::ShaderCreateInfo shader_info;
        shader_info.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        shader_info.Desc.UseCombinedTextureSamplers = true;

        // create a vertex shader
        Diligent::RefCntAutoPtr<Diligent::IShader> vertex_shader;
        {
            shader_info.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            shader_info.EntryPoint = "main";
            shader_info.Desc.Name = "quad vertex shader";
            shader_info.Source = s_vertex_shader_source;
            m_device->CreateShader(shader_info, &vertex_shader);
        }

        // create a pixel shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pixel_shader;
        {
            shader_info.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            shader_info.EntryPoint = "main";
            shader_info.Desc.Name = "quad pixel shader";
            shader_info.Source = s_pixel_shader_source;
            m_device->CreateShader(shader_info, &pixel_shader);
        }

        // create the pipeline state
        pso_info.pVS = vertex_shader;
        pso_info.pPS = pixel_shader;

        pso_info.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        // shader variables should typically be mutable, which means they are expected to change on a per-instance basis
        Diligent::ShaderResourceVariableDesc vars[] =
        {
            {Diligent::SHADER_TYPE_PIXEL, "g_terrain", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
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

        m_pso->CreateShaderResourceBinding(&m_srb, true);
    }

    void application::load_tiff(std::string const& name)
    {
        m_tiff_name = name;
        std::string path = std::string(c_tiffs_dir) + "/" + m_tiff_name;
        m_terrain = std::make_unique<terrain>(path);

        // compute camera information
        {
            float z = std::max(m_terrain->range().b, m_terrain->bounds().as<float>().diagonal().length());
            stff::vec3 eye(0, 0, z);
            m_camera = stff::scamera(eye, stff::constants::half_pi, stff::constants::pi, 0.1f, 10000.f, static_cast<float>(m_width / m_height), stff::scamera::c_default_fov);
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