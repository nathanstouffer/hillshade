#pragma once

#include "hillshader/timer.hpp"

namespace hillshader::camera
{

    struct config
    {
        static constexpr float c_min_terrain_offset = 1.f;
        static constexpr time_t c_default_animation_duration_ms = 300;
    };

}