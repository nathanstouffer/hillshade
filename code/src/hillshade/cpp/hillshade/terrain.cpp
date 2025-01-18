#include "hillshade/terrain.h"

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

        auto tmp0 = dataset->GetProjectionRef();
        auto tmp1 = dataset->GetSpatialRef();
        std::cout << tmp0 << tmp1 << std::endl;
        dataset->GetGeoTransform(m_geo_transform.data());

        // grab dimensions
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();

        // resize data container
        m_values.resize(width * height);

        GDALRasterBand* band = dataset->GetRasterBand(1);
        band->RasterIO(GF_Read, 0, 0, width, height, m_values.data(), width, height, GDT_Float32, 0, 0);

        m_width = static_cast<size_t>(width);
        m_height = static_cast<size_t>(height);

        // compute spatial bounds
        {
            m_top_left = stfd::vec2(m_geo_transform[0], m_geo_transform[3]);
            m_bottom_right = m_top_left;
            m_bottom_right += static_cast<double>(width) * stfd::vec2(m_geo_transform[1], m_geo_transform[4]);
            m_bottom_right += static_cast<double>(height) * stfd::vec2(m_geo_transform[2], m_geo_transform[5]);

            stfd::vec2 center = 0.5 * (m_top_left + m_bottom_right);
            m_top_left -= center;
            m_bottom_right -= center;
        }

        GDALClose(dataset);
    }

}