#pragma once

#include <stf/stf.hpp>

#include "hillshader/terrain.hpp"

namespace hillshader::camera::constrainers
{

    stff::scamera orbit_collide(stff::scamera const& desired, stff::vec3 const& focus, terrain const* terrain);

}