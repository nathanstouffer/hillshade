#pragma once

#include <memory>
#include <optional>
#include <string>

#define NOMINMAX
#include <Windows.h>

#include <imgui.h>

#include <nlohmann/json.hpp>

#include <Common/interface/RefCntAutoPtr.hpp>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Imgui/interface/ImGuiImplWin32.hpp>

#include <stf/stf.hpp>
#include <stf/gfx/color.hpp>

#include "hillshader/camera/controllers/controller.hpp"
#include "hillshader/mesh.hpp"
#include "hillshader/terrain.hpp"

namespace hillshader
{

    enum class focus
    {
        center,
        cursor,
    };

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

        void set_controller(std::unique_ptr<camera::controllers::controller> controller) { m_controller = std::move(controller); }

        void toggle_ui() { m_render_ui = !m_render_ui; }

        inline void reset_camera();

        void zoom(float const factor, focus f);

        void orbit(float const delta_theta, float const delta_phi, focus f);

        void orbit_to(float const theta, float const phi, focus f);
        
        void orbit_attract(float const target_phi, float const rad_per_ms, focus const f);

        inline float aspect_ratio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

        inline void force_focus_update() { m_update_focus = true; }
        ImGuiIO& io() { return ImGui::GetIO(); }
        Diligent::RENDER_DEVICE_TYPE device_type() const { return m_device_type; }

    private:

        Diligent::RefCntAutoPtr<Diligent::IEngineFactory>         m_engine_factory;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>          m_device;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext>         m_immediate_context;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>             m_swap_chain;
        Diligent::RefCntAutoPtr<Diligent::ITexture>               m_msaa_color;
        Diligent::RefCntAutoPtr<Diligent::ITextureView>           m_msaa_color_rtv;
        Diligent::RefCntAutoPtr<Diligent::ITexture>               m_msaa_depth;
        Diligent::RefCntAutoPtr<Diligent::ITextureView>           m_msaa_depth_dsv;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pso;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_srb;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_shader_constants;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_vertex_buffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_index_buffer;
        Diligent::RENDER_DEVICE_TYPE                              m_device_type = Diligent::RENDER_DEVICE_TYPE_GL;
        
        std::unique_ptr<Diligent::ImGuiImplWin32> m_imgui_impl = nullptr;
        Diligent::Uint32 m_width = 1920;
        Diligent::Uint32 m_height = 1080;
        Diligent::Uint8 m_msaa_sample_count = 2;

        bool m_render_ui = true;

        stff::scamera m_camera;
        std::unique_ptr<camera::controllers::controller> m_controller;

        stff::vec3 m_focus;
        bool m_update_focus = false;

        std::string m_dem_path;
        std::unique_ptr<terrain const> m_terrain;
        std::vector<mesh::vertex_t> m_vertices;
        std::vector<uint32_t> m_indices;
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_texture;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_texture_srv;

        stf::gfx::rgba m_clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
        stf::gfx::rgba m_albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
        float m_azimuth = 315.f;
        float m_altitude = 50.f;
        float m_ambient_intensity = 0.0f;
        float m_exaggeration = 5.0f;
        float m_step_scalar = 0.001f;
        bool m_flag_3d = true;

        nlohmann::json m_start_up_state;

    private:

        void store_start_up_state();

        void create_resources();

        void create_msaa_resources();

        void release_msaa_resources();

        void render_ui();

        std::optional<stff::vec3> world_pos(stff::vec2 const& uv) const;

        std::optional<stff::vec3> center_world_pos() const;

        std::optional<stff::vec3> cursor_world_pos() const;

        std::optional<stff::vec3> compute_focus(focus f) const;

        void load_dem(std::string const& path);

        void release_dem_resources();

    };

}