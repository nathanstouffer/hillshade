#include "hillshader/camera/controllers/animators/path.hpp"

#include <algorithm>

namespace hillshader::camera::controllers::animators
{

    path::path() : path(std::vector<input_anchor>()) {}

    path::path(std::vector<input_anchor> anchors) :
          animator(compute_duration(anchors))
    {
        std::stable_sort(anchors.begin(), anchors.end());
        m_anchors.reserve(anchors.size());
        for (auto it = anchors.begin(); it != anchors.end(); ++it)
        {
            input_anchor const& input = *it;
            stff::scamera deriv = path::compute_derivative(anchors, it);
            m_anchors.push_back({ input.timestamp_ms, input.camera, path::overwritten(deriv, input.deriv) });
        }
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
            auto upper = std::upper_bound(m_anchors.begin(), m_anchors.end(), time_ms, [](time_t lhs, anchor const& rhs) { return lhs < rhs.timestamp_ms; });
            anchor const& prev = *(upper - 1);
            anchor const& next = *upper;

            float delta_t = static_cast<float>(next.timestamp_ms - prev.timestamp_ms);
            float t = static_cast<float>(time_ms - prev.timestamp_ms) / delta_t;
            stff::scamera camera = opts.current;
            camera.eye   = stf::math::cubic_hermite_spline(prev.camera.eye,   prev.deriv.eye   * delta_t, next.camera.eye,   next.deriv.eye   * delta_t, t);
            camera.theta = stf::math::cubic_hermite_spline(prev.camera.theta, prev.deriv.theta * delta_t, next.camera.theta, next.deriv.theta * delta_t, t);
            camera.phi   = stf::math::cubic_hermite_spline(prev.camera.phi,   prev.deriv.phi   * delta_t, next.camera.phi,   next.deriv.phi   * delta_t, t);
            return camera;
        }
        else
        {
            return m_anchors.back().camera;
        }
    }

    stff::scamera path::compute_derivative(std::vector<input_anchor> const& anchors, std::vector<input_anchor>::const_iterator it)
    {
        if (it == anchors.cbegin() || it + 1 == anchors.end())
        {
            stff::scamera camera = stff::scamera();
            camera.eye = stff::vec3();
            camera.theta = 0.f;
            camera.phi = 0.f;
            return camera;
        }
        else
        {
            input_anchor const& prev = *(it - 1);
            input_anchor const& curr = *it;
            input_anchor const& next = *(it + 1);

            stff::scamera left_diff = finite_difference(prev, curr);
            stff::scamera right_diff = finite_difference(curr, next);

            float t = static_cast<float>(curr.timestamp_ms - prev.timestamp_ms) / static_cast<float>(next.timestamp_ms - prev.timestamp_ms);

            stff::scamera camera = stff::scamera();
            camera.eye = stf::math::lerp(left_diff.eye, right_diff.eye, t);
            camera.theta = stf::math::lerp(left_diff.theta, right_diff.theta, t);
            camera.phi = stf::math::lerp(left_diff.phi, right_diff.phi, t);
            return camera;
        }
    }

    time_t path::compute_duration(std::vector<input_anchor> const& anchors)
    {
        time_t duration_ms = 0;
        for (input_anchor const& a : anchors)
        {
            duration_ms = std::max(duration_ms, a.timestamp_ms);
        }
        return duration_ms;
    }

    stff::scamera path::finite_difference(input_anchor const& lhs, input_anchor const& rhs)
    {
        float delta_t = static_cast<float>(rhs.timestamp_ms - lhs.timestamp_ms);
        stff::scamera derivative = stff::scamera();
        derivative.eye = (rhs.camera.eye - lhs.camera.eye) / delta_t;
        derivative.theta = (rhs.camera.theta - stf::math::closest_equiv_angle(lhs.camera.theta, rhs.camera.theta)) / delta_t;
        derivative.phi = (rhs.camera.phi - stf::math::closest_equiv_angle(lhs.camera.phi, rhs.camera.phi)) / delta_t;
        return derivative;
    }

    stff::scamera path::overwritten(stff::scamera const& camera, derivative const& deriv)
    {
        stff::scamera cam = camera;
        if (deriv.x.has_value()) cam.eye.x = *deriv.x;
        if (deriv.y.has_value()) cam.eye.y = *deriv.y;
        if (deriv.z.has_value()) cam.eye.z = *deriv.z;
        if (deriv.heading.has_value()) cam.theta = *deriv.heading;
        if (deriv.pitch.has_value()) cam.phi = *deriv.pitch;
        return cam;
    }

}