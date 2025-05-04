#pragma once

#include <array>
#include <filesystem>
#include <optional>

#include <stf/stf.hpp>

namespace hillshader
{

    class terrain
    {
    public:

        terrain(std::filesystem::path const& path);

        float sample(stff::vec2 const& query) const;

        std::optional<stff::vec3> intersect(stff::ray3 const& ray) const;

        inline size_t width() const { return m_width; }
        inline size_t height() const { return m_height; }

        inline stff::interval const& range() const { return m_range; }

        inline stff::aabb2 const& bounds() const { return m_bounds; }

        inline std::vector<float> const& values() const { return m_values; }

    private:

        float read(size_t i, size_t j) const { return m_values[i + m_width * j]; }

        size_t m_width;
        size_t m_height;
        std::vector<float> m_values;

        stff::interval m_range;

        stff::aabb2 m_bounds;

    };

}