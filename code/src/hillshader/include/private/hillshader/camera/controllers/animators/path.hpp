#pragma once

#include <vector>

#include "hillshader/camera/controllers/animators/animator.hpp"

namespace hillshader::camera::controllers::animators
{

    class path final : public animator
    {
    public:

        struct anchor
        {
            stff::scamera camera;
            time_t timestamp_ms;
            inline bool operator<(anchor const& rhs) const { return timestamp_ms < rhs.timestamp_ms; }
        };

    public:

        path();
        path(std::vector<anchor> const& anchors);

    private:

        stff::scamera animator_update(options const& opts) override;

        stff::scamera interpolate(options const& opt);

    private:

        static time_t compute_duration(std::vector<anchor> const& anchors);

    private:

        std::vector<anchor> m_anchors;

    };

}