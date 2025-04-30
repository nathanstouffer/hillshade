#pragma once

#include <vector>

#include <stf/stf.hpp>

namespace hillshader::mesh
{

    struct vertex_t
    {
        stff::vec2 pos;
        stff::vec2 uv;
    };

    // resolution specifies the number of cells along each dimension of the tile
    std::vector<vertex_t> vertices(size_t resolution);
    std::vector<uint32_t> index_list(size_t resolution);
    std::vector<uint32_t> index_strip(size_t resolution);

}