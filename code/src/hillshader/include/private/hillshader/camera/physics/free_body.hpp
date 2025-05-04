#pragma once

#include "hillshader/camera/physics/handler.hpp"
#include "hillshader/timer.hpp"

namespace hillshader::camera::physics
{

    class free_body final : public handler
    {
    public:

        stff::scamera update(options const& opts) override;

    private:

        stff::scamera m_camera;
        stff::vec3 m_velocity;
        time_t m_timestamp_ms;

    };

}