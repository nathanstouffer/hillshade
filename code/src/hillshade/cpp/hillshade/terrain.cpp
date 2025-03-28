#include "hillshade/terrain.hpp"

#include <algorithm>
#include <fstream>

#include <nlohmann/json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace hillshade
{

    terrain::terrain(std::filesystem::path const& path) :
        m_width(0),
        m_height(0)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        unsigned char* img = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (img)
        {
            m_width = static_cast<size_t>(width);
            m_height = static_cast<size_t>(height);

            m_values.reserve(m_width * m_height);

            unsigned char* ptr = img;
            for (int p = 0; p < width * height; ++p)
            {
                float r = static_cast<float>(*ptr++);
                float g = static_cast<float>(*ptr++);
                float b = static_cast<float>(*ptr++);
                float elevation = (r * 256.f + g + b / 256.f) - 32768.f;
                m_values.push_back(elevation);
            }

            stbi_image_free(img);

            // compute elevation range
            {
                m_range = stff::interval(m_values[0], m_values[0]);
                for (float elevation : m_values)
                {
                    m_range.a = std::min(m_range.a, elevation);
                    m_range.b = std::max(m_range.b, elevation);
                }
            }

            // read in metadata
            {
                std::string json_path = path.parent_path().string() + "/" + path.stem().string() + ".json";
                std::ifstream ifs(json_path);
                nlohmann::json json = nlohmann::json::parse(ifs);

                stfd::vec2 min(json["min"][0], json["min"][1]);
                stfd::vec2 max(json["max"][0], json["max"][1]);

                stfd::vec2 center = 0.5 * (min + max);
                min -= center;
                max -= center;

                m_bounds = stfd::aabb2(min, max);
            }
        }
    }

}