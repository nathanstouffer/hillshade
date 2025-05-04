#include "hillshader/camera/controllers/animators/orbit.hpp"

namespace hillshader::camera::controllers::animators
{

    orbit::orbit(stff::scamera const& initial, stff::vec3 const& focus, float delta_theta, float delta_phi, time_t duration_ms) :
          animator(duration_ms)
        , m_initial(initial)
        , m_focus(focus)
        , m_delta_theta(delta_theta)
        , m_delta_phi(delta_phi)
    {}

    stff::scamera orbit::animator_update(options const& opts)
    {
        float t = std::sqrt(progress(opts.time_ms));
        float delta_theta = t * m_delta_theta;
        float delta_phi = t * m_delta_phi;
        return stf::cam::orbit(m_initial, m_focus, delta_phi, delta_theta);
    }

}