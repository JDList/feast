#pragma once

#include "feast/geometry/Geometry.hpp"
#include "feast/geometry/primitives/Cuboid.hpp"

namespace feast{

    class CuboidBuilder
    {
        public:

            Geometry build(const Cuboid& cuboid) const;

    };

}
