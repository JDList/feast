#pragma once

#include "feast/mesh/Mesh.hpp"
#include "feast/meshing/MeshGroups.hpp"

namespace feast
{

class BoundaryFaceCollector
{
public:
    BoundaryFaceCollector() = delete;

    // Fills groups.face_sets from the mesh connectivity and the existing
    // region node sets in groups.node_sets.
    static void collect(MeshGroups& groups, const Mesh& mesh);
};

} // namespace feast
