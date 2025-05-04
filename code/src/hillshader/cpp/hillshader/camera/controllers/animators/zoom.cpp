#include "hillshader/camera/controllers/animators/zoom.hpp"

namespace hillshader::camera::controllers::animators
{

    zoom::zoom(stff::scamera const& initial, stff::vec3 const& focus, float factor, time_t duration_ms) :
          animator(duration_ms)
        , m_initial(initial)
        , m_focus(focus)
        , m_factor(factor)
    {}

    stff::scamera zoom::animator_update(options const& opts)
    {
        float scaler = 1.f / m_factor;
        stff::vec3 final_eye = m_focus + scaler * (m_initial.eye - m_focus);
        float t = std::sqrt(progress(opts.time_ms));
        stff::scamera animated = opts.current;
        animated.eye = stf::math::lerp(m_initial.eye, final_eye, t);
        return animated;
    }

}