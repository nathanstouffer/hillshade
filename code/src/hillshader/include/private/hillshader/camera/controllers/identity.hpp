#pragma once

#include "hillshader/camera/controllers/controller.hpp"

namespace hillshader::camera::controllers
{

    class identity final : public controller
    {
    private:

        stff::scamera derived_update(controller::options const& opts) override { return opts.current; }

    };

}