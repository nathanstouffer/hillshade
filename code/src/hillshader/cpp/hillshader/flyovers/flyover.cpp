#include "hillshader/flyovers/flyover.hpp"

#include <fstream>

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

}