#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    animator::animator(time_t begin_ms, time_t duration_ms) : m_begin_ms(begin_ms), m_duration_ms(duration_ms) {}

    animator::~animator() = default;

    stff::scamera animator::derived_update(options const& opts)
    {
        return (opts.time_ms < m_begin_ms) ? opts.current : animator_update(opts);
    }

    float animator::progress(time_t time_ms) const
    {
        return (time_ms < m_begin_ms) ? 0.f : static_cast<float>(time_ms - m_begin_ms) / static_cast<float>(m_duration_ms);
    }

}