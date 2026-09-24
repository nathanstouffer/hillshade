#include "hillshader/flyovers/parse.hpp"

namespace hillshader::flyovers
{

    void from_json(nlohmann::json const& j, stff::scamera& camera)
    {
        camera.eye.x = j["x"];
        camera.eye.y = j["y"];
        camera.eye.z = j["z"];
        camera.theta = j["heading"];
        camera.phi = j["pitch"];

        camera.theta = stf::math::to_radians(camera.theta);
        camera.phi = stf::math::to_radians(camera.phi);
    }

    void from_json(nlohmann::json const& j, derivative& deriv)
    {
        if (j.contains("x")) deriv.x = j["x"];
        if (j.contains("y")) deriv.y = j["y"];
        if (j.contains("z")) deriv.z = j["z"];
        if (j.contains("heading")) deriv.heading = j["heading"];
        if (j.contains("pitch")) deriv.pitch = j["pitch"];

        if (deriv.heading.has_value()) deriv.heading = stf::math::to_radians(*deriv.heading);
        if (deriv.pitch.has_value()) deriv.pitch = stf::math::to_radians(*deriv.pitch);
    }

    void from_json(nlohmann::json const& j, anchor& anchor)
    {
        anchor.delta = j["delta"];
        from_json(j["camera"], anchor.camera);
        if (j.contains("deriv")) from_json(j["deriv"], anchor.deriv);
    }

}