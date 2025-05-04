#pragma once

#include "hillshader/camera/physics/handler.hpp"
#include "hillshader/timer.hpp"

namespace hillshader::camera::physics
{

    class orbit final : public handler
    {
    public:

        orbit(stff::vec3 const& focus);

        stff::scamera update(options const& opts) override;

    private:

        stff::vec3 m_focus;
        stff::scamera m_camera;
        float m_theta_velocity;
        float m_phi_velocity;
        time_t m_timestamp_ms;

    };

}