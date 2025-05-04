#pragma once

#include "hillshader/camera/controllers/controller.hpp"
#include "hillshader/camera/physics/handler.hpp"

namespace hillshader::camera::controllers
{

    class input final : public controller
    {
    public:

        input(stff::vec3 const& focus);

    private:

        stff::scamera derived_update(options const& opts) override;

        stff::vec3 const m_focus;

        std::unique_ptr<physics::handler> m_physics_handler = nullptr;

    public:

        static bool detect_begin(ImGuiIO const& io);
        static bool detect_active(ImGuiIO const& io);

    };

}