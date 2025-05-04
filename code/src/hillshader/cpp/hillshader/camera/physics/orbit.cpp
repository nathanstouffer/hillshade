#include "hillshader/camera/physics/orbit.hpp"

#include "hillshader/camera/config.hpp"

namespace hillshader::camera::physics
{

    orbit::orbit(stff::vec3 const& focus) : m_focus(focus) {}

    stff::scamera orbit::update(options const& opts)
    {
        float delta_t = static_cast<float>(opts.time_ms - m_timestamp_ms);
        if (m_mode == mode::track)
        {
            m_theta_velocity = (opts.candidate.theta - m_camera.theta) / delta_t;
            m_phi_velocity = (opts.candidate.phi - m_camera.phi) / delta_t;
            m_camera = opts.candidate;
            m_timestamp_ms = opts.time_ms;
            return opts.candidate;
        }
        else if (m_mode == mode::apply)
        {
            float delta_phi = solve_ivp(0.f, m_phi_velocity, config::c_orbit_drag, delta_t);
            float delta_theta = solve_ivp(0.f, m_theta_velocity, config::c_orbit_drag, delta_t);
            return stf::cam::orbit(m_camera, m_focus, delta_phi, delta_theta);
        }
        else
        {
            return opts.candidate;
        }
    }

}