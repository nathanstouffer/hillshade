#include "hillshader/camera/controllers/input.hpp"

#include "hillshader/camera/constraints/collide.hpp"
#include "hillshader/camera/physics/free_body.hpp"
#include "hillshader/camera/physics/orbit.hpp"

namespace hillshader::camera::controllers
{

    static constexpr float c_wheel_scalar = 1.f / (12.5f * 100.f);
    static constexpr float c_pan_scalar = 0.0008125f;

    input::input(stff::vec3 const& focus) : m_focus(focus) {}

    stff::scamera input::derived_update(options const& opts)
    {
        stff::scamera camera = opts.current;

        if (opts.io.MouseWheel != stff::constants::zero)
        {
            float dist = stf::math::dist(camera.eye, m_focus);
            float log_dist = std::log2(dist);
            log_dist -= c_wheel_scalar * opts.io.MouseWheel;
            dist = std::pow(2.f, log_dist);
            stff::vec3 dir = (camera.eye - m_focus).normalize();
            camera.eye = m_focus + dist * dir;
        }

        if (opts.io.MouseDown[0])
        {
            ImVec2 delta = opts.io.MouseDelta;
            float scalar = c_pan_scalar * stf::math::dist(camera.eye, m_focus);
            camera.eye -= scalar * delta.x * camera.right();
            camera.eye += scalar * delta.y * stf::math::cross(stff::vec3(0, 0, 1), camera.right());
            if (!m_physics_handler)
            {
                m_physics_handler = std::make_unique<physics::free_body>();
            }
        }

        if (opts.io.MouseDown[1])
        {
            ImVec2 delta = opts.io.MouseDelta;
            ImVec2 size = opts.io.DisplaySize;
            float delta_theta = -delta.x / size.x * stff::constants::pi;
            float delta_phi = delta.y / size.y * stff::constants::half_pi;
            camera = stf::cam::orbit(camera, m_focus, delta_phi, delta_theta);
            camera = constrainers::orbit_collide(camera, m_focus, opts.terrain);
            if (!m_physics_handler)
            {
                m_physics_handler = std::make_unique<physics::orbit>(m_focus);
            }
        }

        if (opts.io.MouseWheel == stff::constants::zero && !opts.io.MouseDown[0] && !opts.io.MouseDown[1])
        {
            if (m_physics_handler)
            {
                m_physics_handler->set_mode(physics::handler::mode::apply);
            }
        }

        if (m_physics_handler)
        {
            camera = m_physics_handler->update({ camera, opts.time_ms });
        }

        return camera;
    }

}