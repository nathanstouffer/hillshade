#pragma once

#include <vector>

#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    class path final : public animator
    {
    public:

        struct derivative
        {
            std::optional<float> x;
            std::optional<float> y;
            std::optional<float> z;
            std::optional<float> heading;
            std::optional<float> pitch;
        };

        struct input_anchor
        {
            time_t timestamp_ms;
            stff::scamera camera;
            derivative deriv;
            inline bool operator<(input_anchor const& rhs) const { return timestamp_ms < rhs.timestamp_ms; }
        };

    public:

        path();
        path(std::vector<input_anchor> anchors);

    private:

        stff::scamera animator_update(options const& opts) override;

        stff::scamera interpolate(options const& opt) const;

    private:

        static stff::scamera compute_derivative(std::vector<input_anchor> const& anchors, std::vector<input_anchor>::const_iterator it);

        static time_t compute_duration(std::vector<input_anchor> const& anchors);

        static stff::scamera finite_difference(input_anchor const& lhs, input_anchor const& rhs);

        static stff::scamera overwritten(stff::scamera const& camera, derivative const& deriv);

    private:

        struct anchor
        {
            time_t timestamp_ms;
            stff::scamera camera;
            stff::scamera deriv;
        };

        std::vector<anchor> m_anchors;

    };

}