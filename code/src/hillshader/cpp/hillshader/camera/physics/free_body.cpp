#include "hillshader/camera/physics/free_body.hpp"

#include "hillshader/camera/config.hpp"

namespace hillshader::camera::physics
{

    stff::scamera free_body::update(options const& opts)
    {
        float delta_t = static_cast<float>(opts.time_ms - m_timestamp_ms);
        if (m_mode == mode::track)
        {
            m_velocity = (opts.candidate.eye - m_camera.eye) / delta_t;
            m_camera = opts.candidate;
            m_timestamp_ms = opts.time_ms;
            return opts.candidate;
        }
        else if (m_mode == mode::apply)
        {
            stff::scamera camera = opts.candidate;
            camera.eye = solve_ivp(m_camera.eye, m_velocity, config::c_free_body_drag, delta_t);
            return camera;
        }
        else
        {
            return opts.candidate;
        }
    }

}