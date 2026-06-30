#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <vector>

#include "feast/bcs/BoundaryConditionResolver.hpp"
#include "feast/bcs/RegionBoundaryConditions.hpp"
#include "feast/geometry/builders/CuboidBuilder.hpp"
#include "feast/geometry/primitives/Cuboid.hpp"
#include "feast/meshing/StructuredTetMesher.hpp"

static bool sameIds(std::vector<std::size_t> a, std::vector<std::size_t> b)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

int main()
{
    feast::Cuboid cuboid(1.0, 1.0, 1.0);
    feast::CuboidBuilder builder;
    feast::Geometry geometry = builder.build(cuboid);

    feast::StructuredTetMesher mesher(1.0);
    feast::MeshBuildResult buildResult = mesher.generate(geometry);

    feast::RegionBoundaryConditionSet regionBCs;
    regionBCs.addRegionDirichlet("lowx", 0, 0.0);
    regionBCs.addRegionNeumann("highz", 2, 10.0);

    feast::BoundaryConditionSet resolved =
        feast::BoundaryConditionResolver::resolve(regionBCs, buildResult);

    // lowx face of the unit cube should expand to 4 node Dirichlet conditions.
    assert(resolved.dirichletConditions().size() == 4);

    std::vector<std::size_t> dirichletNodes;
    for (const auto& bc : resolved.dirichletConditions()) {
        assert(bc.dof == 0);
        assert(bc.value == 0.0);
        dirichletNodes.push_back(bc.nodeId);
    }
    assert(sameIds(dirichletNodes, {0, 2, 4, 6}));

    // Neumann is area-scaled, so the sum of nodal values should equal traction * area.
    double neumannSum = 0.0;
    std::vector<std::size_t> neumannNodes;
    for (const auto& bc : resolved.neumannConditions()) {
        assert(bc.dof == 2);
        neumannSum += bc.value;
        neumannNodes.push_back(bc.nodeId);
    }

    assert(!resolved.neumannConditions().empty());
    assert(sameIds(neumannNodes, {4, 5, 6, 7}));
    assert(std::abs(neumannSum - 10.0) < 1e-12);

    return 0;
}
