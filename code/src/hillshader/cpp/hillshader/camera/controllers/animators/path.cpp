#include "hillshader/camera/controllers/animators/path.hpp"

#include <algorithm>

namespace hillshader::camera::controllers::animators
{

    path::path() : path(std::vector<anchor>()) {}

    path::path(std::vector<anchor> const& anchors) :
          animator(compute_duration(anchors))
        , m_anchors(anchors)
    {
        std::stable_sort(m_anchors.begin(), m_anchors.end());
    }

    stff::scamera path::animator_update(options const& opts)
    {
        if (m_anchors.empty()) { return opts.current; }
        else if (m_anchors.size() == 1) { return m_anchors.front().camera; }
        else if (begin_ms() + duration_ms() < opts.time_ms)
        {
            time_t time_ms = opts.time_ms - begin_ms();
            // TODO (stouff) figure out if the docs are wrong here
            //auto left_it = std::lower_bound(m_anchors.begin(), m_anchors.end(), time_ms, [](time_t const lhs, anchor const& rhs) { return lhs < rhs.timestamp_ms; });
            //auto right_it = std::upper_bound(m_anchors.begin(), m_anchors.end(), time_ms), [](time_t const lhs, anchor const& rhs) { return lhs < rhs.timestamp_ms; });
            anchor const& left = *std::lower_bound(m_anchors.begin(), m_anchors.end(), time_ms, [](anchor const& lhs, time_t rhs) { return lhs.timestamp_ms < rhs; });
            anchor const& right = *std::upper_bound(m_anchors.begin(), m_anchors.end(), time_ms, [](anchor const& lhs, time_t rhs) { return lhs.timestamp_ms < rhs; });
            float t = static_cast<float>(time_ms - left.timestamp_ms) / static_cast<float>(right.timestamp_ms - left.timestamp_ms);
            return stf::cam::lerp(left.camera, right.camera, t);
        }
        else
        {
            return m_anchors.back().camera;
        }
    }

    time_t path::compute_duration(std::vector<anchor> const& anchors)
    {
        time_t duration_ms = 0;
        for (anchor const& a : anchors)
        {
            duration_ms = std::max(duration_ms, a.timestamp_ms);
        }
        return duration_ms;
    }

}