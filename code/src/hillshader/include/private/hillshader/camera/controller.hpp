#pragma once

#include <stf/stf.hpp>

#include "hillshader/terrain.hpp"

namespace hillshader::camera
{

    class controller
    {
    public:

        struct options
        {
            stff::scamera const& current;
            terrain const* terrain;
        };

    public:

        virtual ~controller();

        stff::scamera update(options const& opts);

    private:

        virtual stff::scamera derived_update(options const& opts) = 0;

    };

}