#include "hillshader/camera/controllers/controller.hpp"

namespace hillshader::camera::controllers
{

    static constexpr float c_min_terrain_offset = 0.5;

    controller::~controller() = default;

    stff::scamera controller::update(options const& opts)
    {
        stff::scamera candidate = derived_update(opts);
        // very basic terrain collision
        if (opts.terrain)
        {
            float threshold = c_min_terrain_offset;
            stff::vec3 eye = candidate.eye;
            if (opts.terrain->bounds().contains(eye.xy))
            {
                threshold += opts.terrain->sample(eye.xy);
            }
            return (eye.z >= threshold) ? candidate : opts.current;
        }
        else
        {
            return (candidate.eye.z >= c_min_terrain_offset) ? candidate : opts.current;
        }
    }

}