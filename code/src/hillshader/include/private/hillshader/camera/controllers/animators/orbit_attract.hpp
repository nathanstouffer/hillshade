#pragma once

#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    class orbit_attract final : public animator
    {
    public:

        orbit_attract(stff::scamera const& initial, stff::vec3 const& focus, float target_phi, float rad_per_ms, time_t duration_ms = 60'000);

    private:

        stff::scamera animator_update(options const& opts) override;

    private:

        stff::scamera m_initial;
        stff::vec3 m_focus;
        float m_target_phi;
        float m_rad_per_ms;

    };

}