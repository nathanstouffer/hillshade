#include "hillshader/terrain.hpp"

#include <algorithm>
#include <fstream>

#include <nlohmann/json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace hillshader
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

                m_bounds = stff::aabb2(min.as<float>(), max.as<float>());
            }
        }
    }

    float terrain::sample(stff::vec2 const& query) const
    {
        stff::aabb2 const bounds = m_bounds.as<float>();
        if (bounds.contains(query))
        {
            stff::vec2 top_left(bounds.min.x, bounds.max.y);
            stff::vec2 bottom_right(bounds.max.x, bounds.min.y);

            stff::vec2 uv = (query - top_left) / (bottom_right - top_left);
            // compute the index of the pixel that is left and up from the sample location
            int i = static_cast<int>(uv.x * m_width  - 0.5f);
            int j = static_cast<int>(uv.y * m_height - 0.5f);

            int clamped_i = std::clamp(i, 0, static_cast<int>(m_width)  - 2);
            int clamped_j = std::clamp(j, 0, static_cast<int>(m_height) - 2);
            if (i != clamped_i || j != clamped_j)   // if we are along a boundary, just use nearest interpolation
            {
                return read(clamped_i, clamped_j);
            }
            else                                    // otherwise, use bilinear interpolation
            {
                // read the four surrounding elevation values
                float a = read(i, j);     float b = read(i + 1, j);
                float c = read(i, j + 1); float d = read(i + 1, j + 1);

                // compute the interpolation times
                float s = stf::math::lerp_inv((i + 0.5f), (i + 1.5f), uv.x * m_width);
                float t = stf::math::lerp_inv((j + 0.5f), (j + 1.5f), uv.y * m_height);

                float top = stf::math::lerp(a, b, s);
                float bottom = stf::math::lerp(c, d, s);

                float elevation = stf::math::lerp(top, bottom, t);
                return elevation;
            }
        }
        else
        {
            return m_range.a;
        }
    }

    std::optional<stff::vec3> terrain::intersect(stff::ray3 const& ray) const
    {
        // this algorithm marches along a ray until the ray crosses the terrain or until the maximum distance is reached
        float initial_step = 0.1f;
        bool initially_above = ray.origin.z >= sample(ray.origin.xy);
        float dist = initial_step;
        for (size_t i = 1; dist <= 100'000.f; ++i)
        {
            size_t scaler = i * i;  // the scaler grows quadratically because detail decreases at that rate
            dist = static_cast<float>(scaler) * initial_step;
            stff::vec3 position = ray.origin + dist * ray.direction;
            bool above = position.z >= sample(position.xy);
            if (above != initially_above)
            {
                return position;
            }
        }

        // fall-through case
        return {};
    }

}