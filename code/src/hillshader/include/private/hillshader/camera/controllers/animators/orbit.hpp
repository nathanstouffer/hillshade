#pragma once

#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    class orbit final : public animator
    {
    public:

        orbit(stff::scamera const& initial, stff::vec3 const& focus, float delta_theta, float delta_phi, time_t duration_ms = 200);

    private:

        stff::scamera animator_update(options const& opts) override;

    private:

        stff::scamera m_initial;
        stff::vec3 m_focus;
        float m_delta_theta;
        float m_delta_phi;

    };

}