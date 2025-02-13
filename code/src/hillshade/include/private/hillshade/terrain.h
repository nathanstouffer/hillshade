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

        void load_tif(std::filesystem::path const& path);
        void load_terrarium(std::filesystem::path const& path);

    private:

        size_t m_width;
        size_t m_height;
        std::vector<float> m_values;

        stff::interval m_range;

        // https://gdal.org/en/release-3.10/tutorials/geotransforms_tut.html#geotransforms-tut
        // [
        //   0: x coordinate of upper-left corner of upper-left pixel
        //   1: west-east pixel resolution / pixel width
        //   2: row rotation (typically zero)
        //   3: y coord of the upper-left corner of the upper-left pixel
        //   4: column rotation (typically zero)
        //   5: north-south pixel resolution / pixel height (negative value for a north-up image)
        // ]
        std::array<double, 6> m_geo_transform;

        stfd::aabb2 m_bounds;

    };

}