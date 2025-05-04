#pragma once

#include "hillshader/timer.hpp"

namespace hillshader::camera
{

    struct config
    {
        static constexpr float c_min_terrain_offset = 1.f;
        static float constexpr c_zoom_factor = 1.5f;
        static float constexpr c_delta_theta = stff::constants::pi_fourths;
        static float constexpr c_delta_phi = 0.5f * stff::constants::pi_fourths;
        static float constexpr c_free_body_drag = 0.01f;
        static float constexpr c_orbit_drag = 0.005f;
    };

}