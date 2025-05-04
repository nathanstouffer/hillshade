#pragma once

#include <imgui.h>

#include <stf/stf.hpp>

#include "hillshader/terrain.hpp"
#include "hillshader/timer.hpp"

namespace hillshader::camera::controllers
{

    class controller
    {
    public:

        struct options
        {
            ImGuiIO const& io;
            stff::scamera const& current;
            terrain const* terrain;
            time_t time_ms;
        };

    public:

        virtual ~controller();

        stff::scamera update(options const& opts);

    private:

        virtual stff::scamera derived_update(options const& opts) = 0;

    };

}