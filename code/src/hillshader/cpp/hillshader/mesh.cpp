#include "hillshader/mesh.hpp"

namespace hillshader::mesh
{

    std::vector<vertex_t> vertices(size_t resolution)
    {
        size_t fenceposts = resolution + 1;
        std::vector<vertex_t> buffer;
        buffer.reserve(fenceposts * fenceposts);

        for (size_t j = 0; j < fenceposts; ++j)
        {
            float y = static_cast<float>(j) / static_cast<float>(resolution);
            for (size_t i = 0; i < fenceposts; ++i)
            {
                float x = static_cast<float>(i) / static_cast<float>(resolution);
                buffer.emplace_back(vertex_t{ stff::vec2(x, y), stff::vec2(x, 1.f - y) });
            }
        }

        return buffer;
    }

    std::vector<uint32_t> index_list(size_t resolution)
    {
        std::vector<uint32_t> buffer;
        buffer.reserve(6 * resolution * resolution);

        size_t fenceposts = resolution + 1;
        size_t offset = 0;
        for (size_t i = 0; i < resolution; ++i)
        {
            for (size_t j = 0; j < resolution; ++j, ++offset)
            {
                buffer.push_back(static_cast<uint32_t>(offset + 0));
                buffer.push_back(static_cast<uint32_t>(offset + fenceposts));
                buffer.push_back(static_cast<uint32_t>(offset + 1));

                buffer.push_back(static_cast<uint32_t>(offset + fenceposts));
                buffer.push_back(static_cast<uint32_t>(offset + fenceposts + 1));
                buffer.push_back(static_cast<uint32_t>(offset + 1));
            }
        }

        return buffer;
    }

    std::vector<uint32_t> index_strip(size_t resolution)
    {
        std::vector<uint32_t> buffer;
        buffer.reserve(resolution * (2 * resolution + 4));

        size_t fenceposts = resolution + 1;
        size_t offset = 0;
        for (size_t i = 0; i < resolution; ++i)
        {
            for (size_t j = 0; j < resolution; ++j, ++offset)
            {
                buffer.push_back(static_cast<uint32_t>(offset + 0));
                buffer.push_back(static_cast<uint32_t>(offset + fenceposts));
            }
            // close triangle row
            buffer.push_back(static_cast<uint32_t>(offset + 0));
            buffer.push_back(static_cast<uint32_t>(offset + fenceposts));

            // add degenerate triangles to close row
            buffer.push_back(static_cast<uint32_t>(offset + fenceposts));
            buffer.push_back(static_cast<uint32_t>(offset + 1));

            ++offset;
        }

        return buffer;
    }

}