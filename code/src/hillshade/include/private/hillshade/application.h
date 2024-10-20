#pragma once

#include <Windows.h>

#include <Common/interface/RefCntAutoPtr.hpp>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Imgui/interface/ImGuiImplWin32.hpp>

namespace hillshade
{

	class application
	{
    public:

        application();
        ~application();
        application(application const& rhs) = delete;
        application& operator=(application const& rhs) = delete;

        bool initialize(HWND hWnd);

        void update();
        void render();
        void present();

        void resize(Diligent::Uint32 width, Diligent::Uint32 height);

        Diligent::RENDER_DEVICE_TYPE device_type() const { return m_device_type; }

    private:

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_device;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_immediate_context;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_swap_chain;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pso;
        Diligent::RENDER_DEVICE_TYPE                      m_device_type = Diligent::RENDER_DEVICE_TYPE_GL;
        
        std::unique_ptr<Diligent::ImGuiImplWin32> m_imgui_impl = nullptr;
        float const m_clear_color[4] = { 0.350f, 0.350f, 0.350f, 1.0f };
        Diligent::Uint32 m_width = 1280;
        Diligent::Uint32 m_height = 1024;

    private:

        void create_resources();

	};

}