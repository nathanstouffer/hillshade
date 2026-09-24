#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <stf/stf.hpp>

#include "hillshader/camera/controllers/controller.hpp"
#include "hillshader/camera/controllers/animators/path.hpp"

namespace hillshader::flyovers
{

    using derivative = typename camera::controllers::animators::path::derivative;

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

        std::unique_ptr<camera::controllers::controller> controller() const;

    private:

        std::string m_dem;
        stff::scamera m_initial;
        std::vector<anchor> m_anchors;

    };

}