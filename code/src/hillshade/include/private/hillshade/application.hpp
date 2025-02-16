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

#include <stf/stf.hpp>
#include <stf/gfx/color.hpp>

#include "hillshade/mesh.hpp"
#include "hillshade/terrain.hpp"

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

        inline void zoom_in(float const factor) { m_camera.eye.z /= factor; }
        inline void zoom_out(float const factor) { m_camera.eye.z *= factor; }
        inline void pan(stff::vec2 const& factor) { m_camera.eye.xy += m_camera.eye.z * factor; }

        inline float aspect_ratio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

        ImGuiIO& io() { return ImGui::GetIO(); }
        Diligent::RENDER_DEVICE_TYPE device_type() const { return m_device_type; }

    private:

        Diligent::RefCntAutoPtr<Diligent::IEngineFactory>         m_engine_factory;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>          m_device;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext>         m_immediate_context;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>             m_swap_chain;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pso;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_shader_constants;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_vertex_buffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_index_buffer;
        Diligent::RENDER_DEVICE_TYPE                              m_device_type = Diligent::RENDER_DEVICE_TYPE_GL;
        
        std::unique_ptr<Diligent::ImGuiImplWin32> m_imgui_impl = nullptr;
        Diligent::Uint32 m_width = 1280;
        Diligent::Uint32 m_height = 1024;

        bool m_render_ui = true;

        stff::scamera m_camera;

        std::string m_dem_path;
        std::unique_ptr<terrain> m_terrain;
        std::vector<mesh::vertex_t> m_vertices;
        std::vector<uint32_t> m_indices;
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_texture;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_texture_srv;

        stf::gfx::rgba m_clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        stf::gfx::rgba m_albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
        float m_azimuth = 315.f;
        float m_altitude = 50.f;
        float m_ambient_intensity = 0.0f;
        float m_exaggeration = 2.0f;
        float m_step_scalar = 0.001f;
        bool m_flag_3d = true;

    private:

        void create_resources();

        void render_ui();

        void load_dem(std::string const& path);

    };

}