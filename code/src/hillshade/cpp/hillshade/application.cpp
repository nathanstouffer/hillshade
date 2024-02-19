#include "hillshade/application.h"

#include <Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>

namespace hillshade
{

    static const char* s_vertex_shader_source = R"(
        struct PSInput 
        { 
            float4 pos   : SV_POSITION; 
            float3 color : COLOR; 
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

            pixel_input.pos   = positions[vertex_id];
            pixel_input.color = colors[vertex_id];
        }
    )";

    static const char* s_pixel_shader_source = R"(
        struct PSInput 
        { 
            float4 pos   : SV_POSITION; 
            float3 color : COLOR; 
        };

        struct PSOutput
        { 
            float4 color : SV_TARGET; 
        };

        void main(in PSInput pixel_input, out PSOutput pixel_output)
        {
            pixel_output.color = float4(pixel_input.color.rgb, 1.0);
        }
    )";

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

        create_resources();

        return true;
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
        m_device->CreateGraphicsPipelineState(pso_info, &m_pso);
    }

    void application::render()
    {
        // set render targets before issuing any draw command.
        // note that present() unbinds the back buffer if it is set as render target.
        auto* rtv = m_swap_chain->GetCurrentBackBufferRTV();
        auto* dsv = m_swap_chain->GetDepthBufferDSV();
        m_immediate_context->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // clear back buffer
        float const clear_color[] = { 0.350f, 0.350f, 0.350f, 1.0f };
        m_immediate_context->ClearRenderTarget(rtv, clear_color, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_immediate_context->ClearDepthStencil(dsv, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // set the pipeline state in the immediate context
        m_immediate_context->SetPipelineState(m_pso);

        Diligent::DrawAttribs draw_attrs;
        draw_attrs.NumVertices = 6;
        m_immediate_context->Draw(draw_attrs);
    }

    void application::present()
    {
        m_swap_chain->Present();
    }

    void application::resize(Diligent::Uint32 width, Diligent::Uint32 height)
    {
        if (m_swap_chain)
        {
            m_swap_chain->Resize(width, height);
        }
    }

}