#pragma once

#include <filesystem>

namespace hillshade
{

    class terrain
    {
    public:

        terrain(std::filesystem::path const& path);

        inline size_t width() const { return m_width; }
        inline size_t height() const { return m_height; }
        inline std::vector<float> const& values() const { return m_values; }

    private:

        size_t m_width;
        size_t m_height;
        std::vector<float> m_values;

    };

}