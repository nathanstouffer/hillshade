#pragma once

#include "hillshader/camera/controllers/controller.hpp"

namespace hillshader::camera::controllers::reactors
{

    class input final : public controller
    {
    public:

        input(stff::vec3 const& focus);

    private:

        stff::scamera derived_update(options const& opts) override;

        stff::vec3 const m_focus;

    };

}