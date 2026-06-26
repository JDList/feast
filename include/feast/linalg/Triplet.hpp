#pragma once

#include <cstddef>

namespace feast {

struct Triplet
{
    std::size_t row{0};
    std::size_t col{0};
    double value{0.0};
};

} // namespace feast
