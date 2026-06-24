#pragma once

#include <cstddef>
#include <vector>
#include "feast/mesh/ElementType.hpp"

namespace feast {

struct Element {
    std::size_t id;
    ElementType type;
    std::vector<std::size_t> node_ids;
    std::size_t material_id;
};

}
