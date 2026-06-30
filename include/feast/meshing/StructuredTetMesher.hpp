#pragma once

#include <cstddef>

#include "feast/geometry/Geometry.hpp"
#include "feast/meshing/MeshBuildResult.hpp"

namespace feast
{

class StructuredTetMesher
{
public:
    explicit StructuredTetMesher(double target_element_size);

    double targetElementSize() const;

    MeshBuildResult generate(const Geometry& geometry) const;

private:
    double target_element_size_;
};

} // namespace feast
