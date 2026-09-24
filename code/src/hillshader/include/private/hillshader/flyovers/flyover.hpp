#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <string>

#include <stf/stf.hpp>

namespace hillshader::flyovers
{

    struct derivative
    {
        std::optional<float> x;
        std::optional<float> y;
        std::optional<float> z;
        std::optional<float> heading;
        std::optional<float> pitch;
    };

    struct anchor
    {
        float delta;
        stff::scamera camera;
        derivative deriv;
    };

    class flyover
    {
    public:
        flyover(std::filesystem::path const& path);

        std::string const& dem() const { return m_dem; }

    private:

        std::string m_dem;
        stff::scamera m_initial;
        std::vector<anchor> m_anchors;

    };

}