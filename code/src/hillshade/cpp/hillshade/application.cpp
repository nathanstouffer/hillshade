#include "hillshade/application.h"

#include <memory>
#include <iomanip>
#include <iostream>

#include <Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>

namespace hillshade
{

    static const char* VSSource = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        void main(in  uint    VertId : SV_VertexID,
                  out PSInput PSIn) 
        {
            float4 Pos[6];
            Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
            Pos[1] = float4(-0.5, +0.5, 0.0, 1.0);
            Pos[2] = float4(+0.5, +0.5, 0.0, 1.0);
            Pos[3] = float4(+0.5, +0.5, 0.0, 1.0);
            Pos[4] = float4(+0.5, -0.5, 0.0, 1.0);
            Pos[5] = float4(-0.5, -0.5, 0.0, 1.0);

            float3 Col[6];
            Col[0] = float3(1.0, 0.0, 0.0);
            Col[1] = float3(1.0, 1.0, 0.0);
            Col[2] = float3(0.0, 1.0, 0.0);
            Col[3] = float3(0.0, 1.0, 0.0);
            Col[4] = float3(1.0, 1.0, 0.0);
            Col[5] = float3(1.0, 0.0, 0.0);

            PSIn.Pos   = Pos[VertId];
            PSIn.Color = Col[VertId];
        }
    )";

    // Pixel shader simply outputs interpolated vertex color
    static const char* PSSource = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        struct PSOutput
        { 
            float4 Color : SV_TARGET; 
        };

        void main(in  PSInput  PSIn,
                  out PSOutput PSOut)
        {
            PSOut.Color = float4(PSIn.Color.rgb, 1.0);
        }
    )";

    application::~application()
    {
        m_immediate_context->Flush();
    }

    bool application::initialize(HWND hWnd)
    {
        Diligent::SwapChainDesc SCDesc;
#    if EXPLICITLY_LOAD_ENGINE_GL_DLL
        // Load the dll and import GetEngineFactoryOpenGL() function
        auto GetEngineFactoryOpenGL = Diligent::LoadGraphicsEngineOpenGL();
#    endif
        auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

        Diligent::EngineGLCreateInfo EngineCI;
        EngineCI.Window.hWnd = hWnd;

        pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_device, &m_immediate_context, SCDesc, &m_swap_chain);
        return true;
    }

    void application::create_resources()
    {
        // Pipeline state object encompasses configuration of all GPU stages

        Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;

        // Pipeline state name is used by the engine to report issues.
        // It is always a good idea to give objects descriptive names.
        PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

        // This is a graphics pipeline
        PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        // clang-format off
        // This tutorial will render to a single render target
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        // Set render target format which is the format of the swap chain's color buffer
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_swap_chain->GetDesc().ColorBufferFormat;
        // Use the depth buffer format from the swap chain
        PSOCreateInfo.GraphicsPipeline.DSVFormat = m_swap_chain->GetDesc().DepthBufferFormat;
        // Primitive topology defines what kind of primitives will be rendered by this pipeline state
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // No back face culling for this tutorial
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        // Disable depth testing
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::False;
        // clang-format on

        Diligent::ShaderCreateInfo ShaderCI;
        // Tell the system that the shader source code is in HLSL.
        // For OpenGL, the engine will convert this into GLSL under the hood
        ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.Desc.UseCombinedTextureSamplers = true;
        // Create a vertex shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
        {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = "Triangle vertex shader";
            ShaderCI.Source = VSSource;
            m_device->CreateShader(ShaderCI, &pVS);
        }

        // Create a pixel shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
        {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = "Triangle pixel shader";
            ShaderCI.Source = PSSource;
            m_device->CreateShader(ShaderCI, &pPS);
        }

        // Finally, create the pipeline state
        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;
        m_device->CreateGraphicsPipelineState(PSOCreateInfo, &m_pso);
    }

    void application::render()
    {
        // Set render targets before issuing any draw command.
        // Note that Present() unbinds the back buffer if it is set as render target.
        auto* pRTV = m_swap_chain->GetCurrentBackBufferRTV();
        auto* pDSV = m_swap_chain->GetDepthBufferDSV();
        m_immediate_context->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Clear the back buffer
        const float ClearColor[] = { 0.350f, 0.350f, 0.350f, 1.0f };
        // Let the engine perform required state transitions
        m_immediate_context->ClearRenderTarget(pRTV, ClearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_immediate_context->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Set the pipeline state in the immediate context
        m_immediate_context->SetPipelineState(m_pso);

        // Typically we should now call CommitShaderResources(), however shaders in this example don't
        // use any resources.

        Diligent::DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 6; // Render 6 vertices
        m_immediate_context->Draw(drawAttrs);
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