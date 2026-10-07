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
        m_delay_ms = static_cast<time_t>(1000.f * static_cast<float>(json["delay"]));
        from_json(json["anchors"], m_anchors);
    }

    std::unique_ptr<camera::controllers::animators::animator> flyover::controller() const
    {
        std::vector<camera::controllers::animators::path::input_anchor> anchors;
        anchors.reserve(1 + m_anchors.size());

        time_t duration = 0;
        anchors.push_back({ duration, m_initial, { 0.f, 0.f, 0.f, 0.f, 0.f } });
        for (anchor const& a : m_anchors)
        {
            duration += static_cast<time_t>(a.delta);
            anchors.push_back({ duration, a.camera, a.deriv });
        }

        return std::make_unique<camera::controllers::animators::path>(anchors, m_delay_ms);
    }

}