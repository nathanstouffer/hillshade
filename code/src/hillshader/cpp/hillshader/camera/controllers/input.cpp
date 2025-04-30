#include "hillshader/camera/controllers/input.hpp"

namespace hillshader::camera::controllers
{

    static constexpr float c_wheel_scalar = 1.f / (12.5f * 120.f);
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
        }

        if (opts.io.MouseDown[1])
        {
            ImVec2 delta = opts.io.MouseDelta;
            ImVec2 size = opts.io.DisplaySize;
            float delta_theta = -delta.x / size.x * stff::constants::pi;
            float delta_phi = delta.y / size.y * stff::constants::half_pi;
            camera = stf::cam::orbit(camera, m_focus, delta_phi, delta_theta);
        }

        return camera;
    }

}