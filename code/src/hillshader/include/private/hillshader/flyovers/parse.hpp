#pragma once

#include <nlohmann/json.hpp>

#include "hillshader/flyovers/flyover.hpp"

namespace hillshader::flyovers
{

    void from_json(nlohmann::json const& j, stff::scamera& camera);
    void from_json(nlohmann::json const& j, derivative& deriv);
    void from_json(nlohmann::json const& j, anchor& anchor);

}