#pragma once

#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    class zoom final : public animator
    {
    public:

        zoom(stff::scamera const& initial, stff::vec3 const& focus, float scaler, time_t duration_ms);

    private:

        stff::scamera animator_update(options const& opts) override;

    private:

        stff::scamera m_initial;
        stff::vec3 m_focus;
        float m_scaler;

    };

}