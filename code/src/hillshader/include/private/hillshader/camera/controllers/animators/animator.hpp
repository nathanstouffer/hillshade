#pragma once

#include "hillshader/camera/controllers/controller.hpp"

namespace hillshader::camera::controllers::animators
{

    class animator : public controller
    {
    public:

        animator(time_t begin_ms, time_t duration_ms);
        virtual ~animator();

        void restart(time_t const begin_ms) { m_begin_ms = begin_ms; }

    protected:

        float progress(time_t time_ms) const;

    private:

        stff::scamera derived_update(options const& opts) override;

        virtual stff::scamera animator_update(options const& opts) = 0;

    private:

        time_t m_begin_ms;
        time_t m_duration_ms;

    };

}