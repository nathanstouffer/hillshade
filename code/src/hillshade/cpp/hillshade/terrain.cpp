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

        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();
        std::cout << "Width: " << width << ", Height: " << height << std::endl;

        GDALClose(dataset);
    }

}