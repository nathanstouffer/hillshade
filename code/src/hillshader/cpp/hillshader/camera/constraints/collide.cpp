#include "hillshader/camera/constraints/collide.hpp"

#include "hillshader/camera/config.hpp"

namespace hillshader::camera::constrainers
{

    stff::scamera orbit_collide(stff::scamera const& desired, stff::vec3 const& focus, terrain const* terrain)
    {
        if (std::abs(desired.phi) < stff::constants::tol)
        {
            return desired;
        }
        else
        {
            float threshold = config::c_min_terrain_offset;
            if (terrain)
            {
                threshold += terrain->sample(desired.eye.xy);
            }
            
            if (desired.eye.z >= threshold)
            {
                return desired;
            }
            else
            {
                float phi = desired.phi * 0.999f;
                float delta_phi = desired.phi - phi;
                return orbit_collide(stf::cam::orbit(desired, focus, delta_phi, 0.f), focus, terrain);
            }
        }
    }

}