#pragma once

#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/bcs/RegionBoundaryConditions.hpp"
#include "feast/meshing/MeshBuildResult.hpp"

namespace feast {

class BoundaryConditionResolver
{
public:
    BoundaryConditionResolver() = delete;

    // Expands both region Dirichlet and region Neumann conditions into
    // node-based BoundaryConditionSet entries.
    //
    // Dirichlet:
    //   region -> node set -> node Dirichlet conditions
    //
    // Neumann:
    //   region -> boundary faces -> area-weighted nodal Neumann conditions
    static BoundaryConditionSet resolve(
        const RegionBoundaryConditionSet& regionConditions,
        const MeshBuildResult& buildResult);

    static BoundaryConditionSet resolveDirichlet(
        const RegionBoundaryConditionSet& regionConditions,
        const MeshBuildResult& buildResult);

    static BoundaryConditionSet resolveNeumann(
        const RegionBoundaryConditionSet& regionConditions,
        const MeshBuildResult& buildResult);
};

} // namespace feast
