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
        stff::scamera interpolated = interpolate(opts);
        stff::scamera camera = opts.current;
        camera.eye = interpolated.eye;
        camera.theta = interpolated.theta;
        camera.phi = interpolated.phi;
        return camera;
    }

    stff::scamera path::interpolate(options const& opts) const
    {
        if (m_anchors.empty()) { return opts.current; }
        else if (m_anchors.size() == 1) { return m_anchors.front().camera; }
        else if (opts.time_ms < end_ms())
        {
            time_t time_ms = opts.time_ms - begin_ms();
            // TODO (stouff) possibly use upper bound and lower bound?
            auto next = std::upper_bound(m_anchors.begin(), m_anchors.end(), time_ms, [](time_t lhs, anchor const& rhs) { return lhs < rhs.timestamp_ms; });
            auto left_iter = next - 1;
            auto right_iter = next;

            stff::scamera left_deriv = derivative(left_iter);
            stff::scamera right_deriv = derivative(right_iter);

            float delta_t = static_cast<float>(right_iter->timestamp_ms - left_iter->timestamp_ms);
            float t = static_cast<float>(time_ms - left_iter->timestamp_ms) / delta_t;
            stff::scamera camera = opts.current;
            camera.eye   = stf::math::cubic_hermite_spline(left_iter->camera.eye,   left_deriv.eye   * delta_t, right_iter->camera.eye,   right_deriv.eye   * delta_t, t);
            camera.theta = stf::math::cubic_hermite_spline(left_iter->camera.theta, left_deriv.theta * delta_t, right_iter->camera.theta, right_deriv.theta * delta_t, t);
            camera.phi   = stf::math::cubic_hermite_spline(left_iter->camera.phi,   left_deriv.phi   * delta_t, right_iter->camera.phi,   right_deriv.phi   * delta_t, t);
            return camera;
        }
        else
        {
            return m_anchors.back().camera;
        }
    }

    stff::scamera path::derivative(std::vector<anchor>::const_iterator it) const
    {
        if (it == m_anchors.cbegin() || it + 1 == m_anchors.end())
        {
            stff::scamera camera = stff::scamera();
            camera.eye = stff::vec3();
            camera.theta = 0.f;
            camera.phi = 0.f;
            return camera;
        }
        else
        {
            anchor const& prev = *(it - 1);
            anchor const& curr = *it;
            anchor const& next = *(it + 1);

            stff::scamera left_deriv = finite_difference(prev, curr);
            stff::scamera right_deriv = finite_difference(curr, next);

            float t = (curr.timestamp_ms - prev.timestamp_ms) / (next.timestamp_ms - prev.timestamp_ms);

            stff::scamera camera = stff::scamera();
            camera.eye = stf::math::lerp(left_deriv.eye, right_deriv.eye, t);
            camera.theta = stf::math::lerp(left_deriv.theta, right_deriv.theta, t);
            camera.phi = stf::math::lerp(left_deriv.phi, right_deriv.phi, t);
            return camera;
        }
    }

    stff::scamera path::finite_difference(anchor const& lhs, anchor const& rhs) const
    {
        float delta_t = static_cast<float>(rhs.timestamp_ms - lhs.timestamp_ms);
        stff::scamera derivative = stff::scamera();
        derivative.eye = (rhs.camera.eye - lhs.camera.eye) / delta_t;
        derivative.theta = (rhs.camera.theta - stf::math::closest_equiv_angle(lhs.camera.theta, rhs.camera.theta)) / delta_t;
        derivative.phi = (rhs.camera.phi - stf::math::closest_equiv_angle(lhs.camera.phi, rhs.camera.phi)) / delta_t;
        return derivative;
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