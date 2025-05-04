#pragma once

#include <stf/stf.hpp>

namespace hillshader::camera::physics
{

    class handler
    {
    public:

        enum class mode
        {
            track,
            apply,
        };

        struct options
        {
            stff::scamera const& candidate;
            time_t time_ms;
        };

    public:

        virtual ~handler();

        void set_mode(mode m) { m_mode = m; }

        virtual stff::scamera update(options const& opts) = 0;

    protected:

        mode m_mode = mode::track;

        template<typename T>
        T solve_ivp(T const& initial, T const& velocity, float drag, float time_ms) const
        {
            float scaler = -(std::exp(-drag * time_ms) - 1.f) / drag;
            return initial + scaler * velocity;
        }

    };

}