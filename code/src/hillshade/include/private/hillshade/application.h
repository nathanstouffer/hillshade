#pragma once

#include <string>

#define NOMINMAX
#include <Windows.h>

#include <imgui.h>

#include <Common/interface/RefCntAutoPtr.hpp>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Imgui/interface/ImGuiImplWin32.hpp>

#include <stf/gfx/color.hpp>

#include "hillshade/terrain.h"

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

        void toggle_ui() { m_render_ui = !m_render_ui; }

        ImGuiIO& io() { return ImGui::GetIO(); }
        Diligent::RENDER_DEVICE_TYPE device_type() const { return m_device_type; }

    private:

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_device;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_immediate_context;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_swap_chain;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pso;
        Diligent::RENDER_DEVICE_TYPE                      m_device_type = Diligent::RENDER_DEVICE_TYPE_GL;
        
        std::unique_ptr<Diligent::ImGuiImplWin32> m_imgui_impl = nullptr;
        Diligent::Uint32 m_width = 1280;
        Diligent::Uint32 m_height = 1024;

        bool m_render_ui = true;

        std::string m_tiff_name;
        std::unique_ptr<terrain> m_terrain;

        stf::gfx::rgba m_clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        stf::gfx::rgba m_albedo = { 1.0f, 1.0f, 1.0f, 1.0f };

    private:

        void create_resources();

        void render_ui();

        void load_tiff(std::string const& name);

	};

}