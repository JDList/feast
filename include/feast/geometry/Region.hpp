#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "feast/geometry/FaceTag.hpp"

namespace feast
{

enum class RegionType
{
    Face,
    Edge,
    Vertex,
    Volume
};

struct Region
{
    std::string name;
    RegionType type;
    FaceTag tag;
    std::vector<std::size_t> entity_ids;
};

} // namespace feast
