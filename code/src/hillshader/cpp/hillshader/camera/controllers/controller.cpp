#include "hillshader/camera/controllers/controller.hpp"

#include "hillshader/camera/config.hpp"

namespace hillshader::camera::controllers
{

    controller::~controller() = default;

    stff::scamera controller::update(options const& opts)
    {
        stff::scamera candidate = derived_update(opts);
        
        // very basic terrain collision as a double check
        float threshold = config::c_min_terrain_offset;
        if (opts.terrain)
        {
            threshold += opts.terrain->sample(candidate.eye.xy);
        }

        candidate = (candidate.eye.z >= threshold) ? candidate : opts.current;
    
        candidate.theta = stf::math::canonical_angle(candidate.theta);
        candidate.phi = stf::math::canonical_angle(candidate.phi);
        return candidate;
    }

}