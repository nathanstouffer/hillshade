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

        // grab dimensions
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();

        // resize data container
        m_values.resize(width * height);

        GDALRasterBand* band = dataset->GetRasterBand(1);
        band->RasterIO(GF_Read, 0, 0, width, height, m_values.data(), width, height, GDT_Float32, 0, 0);

        m_width = static_cast<size_t>(width);
        m_height = static_cast<size_t>(height);

        GDALClose(dataset);
    }

}