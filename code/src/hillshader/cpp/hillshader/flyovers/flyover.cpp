#include "hillshader/flyovers/flyover.hpp"

#include <fstream>

#include "hillshader/camera/controllers/animators/path.hpp"
#include "hillshader/flyovers/parse.hpp"

namespace hillshader::flyovers
{

    flyover::flyover(std::filesystem::path const& path)
    {
        std::ifstream ifs(path);
        nlohmann::json json = nlohmann::json::parse(ifs);

        m_dem = json["dem"];
        from_json(json["initial"], m_initial);
        from_json(json["anchors"], m_anchors);
    }

    std::unique_ptr<camera::controllers::controller> flyover::controller() const
    {
        std::vector<camera::controllers::animators::path::input_anchor> anchors;
        anchors.reserve(1 + m_anchors.size());

        time_t duration = 0;
        anchors.push_back({ duration, m_initial, { 0, 0, 0, 0, 0 } });
        for (anchor const& a : m_anchors)
        {
            duration += a.delta;
            anchors.push_back({ duration, a.camera, a.deriv });
        }

        return std::make_unique<camera::controllers::animators::path>(anchors);
    }

}