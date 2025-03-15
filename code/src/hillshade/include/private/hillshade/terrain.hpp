#pragma once

#include <array>
#include <filesystem>

#include <stf/stf.hpp>

namespace hillshade
{

    class terrain
    {
    public:

        terrain(std::filesystem::path const& path);

        inline size_t width() const { return m_width; }
        inline size_t height() const { return m_height; }

        inline stff::interval const& range() const { return m_range; }

        inline stfd::aabb2 const& bounds() const { return m_bounds; }

        inline std::vector<float> const& values() const { return m_values; }

    private:

        size_t m_width;
        size_t m_height;
        std::vector<float> m_values;

        stff::interval m_range;

        stfd::aabb2 m_bounds;

    };

}