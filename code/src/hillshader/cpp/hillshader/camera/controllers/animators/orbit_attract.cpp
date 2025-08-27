#include "hillshader/camera/controllers/animators/orbit_attract.hpp"

namespace hillshader::camera::controllers::animators
{

    orbit_attract::orbit_attract(stff::scamera const& initial, stff::vec3 const& focus, float target_phi, float rad_per_ms, time_t duration_ms) :
        animator(duration_ms)
        , m_initial(initial)
        , m_focus(focus)
        , m_rad_per_ms(rad_per_ms)
        , m_target_phi(target_phi)
    {}

    stff::scamera orbit_attract::animator_update(options const& opts)
    {
        float constexpr extreme_phi = 2.f * stff::constants::pi / 3.f;
        float constexpr delay = 500.f;
        float constexpr threshold0 = 2'000.f + delay;
        float constexpr threshold1 = threshold0 + 1'000.f + delay;

        float delta_t = static_cast<float>(opts.time_ms - begin_ms());
        if (delta_t <= threshold0)
        {
            float t = std::min(delta_t / static_cast<float>(threshold0 - delay), 1.f);
            t = stf::math::smooth_time(t);
            float phi = stf::math::lerp(m_initial.phi, extreme_phi, t);
            return stf::cam::orbit(m_initial, m_focus, phi - m_initial.phi, 0.f);

        }
        else if (delta_t <= threshold1)
        {
            delta_t -= threshold0;
            float t = std::min(delta_t / static_cast<float>(threshold1 - delay - threshold0), 1.f);
            t = stf::math::smooth_time(t);
            float phi = stf::math::lerp(extreme_phi, m_target_phi, t);
            return stf::cam::orbit(m_initial, m_focus, phi - m_initial.phi, 0.f);
        }
        else
        {
            float delta_phi = m_target_phi - m_initial.phi;
            float delta_theta = std::max(0.f, delta_t - threshold1) * m_rad_per_ms;
            return stf::cam::orbit(m_initial, m_focus, delta_phi, delta_theta);
        }
    }

}