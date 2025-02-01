#include "hillshade/terrain.h"

#include <algorithm>

#include <gdal_priv.h>

namespace hillshade
{

    terrain::terrain(std::filesystem::path const& path)
    {
        GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpen(path.string().c_str(), GA_ReadOnly));

        if (dataset == nullptr)
        {
            std::cerr << "Error opening file: " << path << std::endl;
            return;
        }

        dataset->GetGeoTransform(m_geo_transform.data());

        // grab dimensions
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();

        // resize data container
        m_values.resize(width * height);

        // TODO (stouff) improve run time load on this
        GDALRasterBand* band = dataset->GetRasterBand(1);
        band->RasterIO(GF_Read, 0, 0, width, height, m_values.data(), width, height, GDT_Float32, 0, 0);

        m_width = static_cast<size_t>(width);
        m_height = static_cast<size_t>(height);

        // compute elevation range
        {
            m_range = stff::interval(m_values[0], m_values[0]);
            for (float elevation : m_values)
            {
                m_range.a = std::min(m_range.a, elevation);
                m_range.b = std::max(m_range.b, elevation);
            }
        }

        // compute spatial bounds
        {
            stfd::vec2 top_left = stfd::vec2(m_geo_transform[0], m_geo_transform[3]);
            stfd::vec2 bottom_right = top_left;
            bottom_right += static_cast<double>(width) * stfd::vec2(m_geo_transform[1], m_geo_transform[4]);
            bottom_right += static_cast<double>(height) * stfd::vec2(m_geo_transform[2], m_geo_transform[5]);

            stfd::vec2 center = 0.5 * (top_left + bottom_right);
            top_left -= center;
            bottom_right -= center;

            m_bounds = stfd::aabb2(stfd::vec2(top_left.x, bottom_right.y), stfd::vec2(bottom_right.x, top_left.y));
        }

        GDALClose(dataset);
    }

}