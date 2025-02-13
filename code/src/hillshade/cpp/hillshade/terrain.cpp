#include "hillshade/terrain.h"

#include <algorithm>
#include <fstream>

#include <gdal_priv.h>

#include <nlohmann/json.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace hillshade
{

    terrain::terrain(std::filesystem::path const& path) :
        m_width(0),
        m_height(0),
        m_geo_transform({})
    {
        std::string filename = path.filename().string();
        size_t dot = filename.find(".");
        std::string_view extension = std::string_view(filename).substr(dot + 1);
        if (extension == "tif" || extension == "tiff" || extension == "geotiff")
        {
            load_tif(path);
        }
        else
        {
            load_terrarium(path);
        }
    }

    void terrain::load_tif(std::filesystem::path const& path)
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

    void terrain::load_terrarium(std::filesystem::path const& path)
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