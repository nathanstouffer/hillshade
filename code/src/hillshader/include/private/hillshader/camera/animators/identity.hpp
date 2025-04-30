#pragma once

#include "hillshader/camera/controller.hpp"

namespace hillshader::camera::animators
{

    class identity final : public controller
    {
    private:

        stff::scamera derived_update(controller::options const& opts) override { return opts.current; }

    };

}