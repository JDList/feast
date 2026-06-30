#pragma once 

#include "feast/mesh/Mesh.hpp"
#include "feast/meshing/MeshGroups.hpp"

namespace feast{

struct MeshBuildResult{

    Mesh mesh;
    MeshGroups groups;

};
}
