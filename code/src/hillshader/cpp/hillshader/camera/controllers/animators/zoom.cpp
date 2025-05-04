#include "hillshader/camera/controllers/animators/zoom.hpp"

namespace hillshader::camera::controllers::animators
{

    zoom::zoom(stff::scamera const& initial, stff::vec3 const& focus, float scaler, time_t duration_ms) : 
          animator(duration_ms)
        , m_initial(initial)
        , m_focus(focus)
        , m_scaler(scaler)
    {}

    stff::scamera zoom::animator_update(options const& opts)
    {
        stff::vec3 final_eye = m_focus + m_scaler * (m_initial.eye - m_focus);
        float t = progress(opts.time_ms);
        stff::scamera animated = opts.current;
        animated.eye = stf::math::lerp(m_initial.eye, final_eye, t);
        return animated;
    }

}