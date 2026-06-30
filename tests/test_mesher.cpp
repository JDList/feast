#include <algorithm>
#include <cassert>
#include <vector>

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
    feast::MeshBuildResult result = mesher.generate(geometry);

    assert(result.mesh.nodes().size() == 8);
    assert(result.mesh.elements().size() == 6);

    assert(result.groups.node_sets.count("lowx") == 1);
    assert(result.groups.node_sets.count("highx") == 1);
    assert(result.groups.node_sets.count("lowy") == 1);
    assert(result.groups.node_sets.count("highy") == 1);
    assert(result.groups.node_sets.count("lowz") == 1);
    assert(result.groups.node_sets.count("highz") == 1);

    assert(result.groups.node_sets.at("lowx").size() == 4);
    assert(result.groups.node_sets.at("highx").size() == 4);
    assert(result.groups.node_sets.at("lowy").size() == 4);
    assert(result.groups.node_sets.at("highy").size() == 4);
    assert(result.groups.node_sets.at("lowz").size() == 4);
    assert(result.groups.node_sets.at("highz").size() == 4);

    assert(sameIds(result.groups.node_sets.at("lowx"),  {0, 2, 4, 6}));
    assert(sameIds(result.groups.node_sets.at("highx"), {1, 3, 5, 7}));
    assert(sameIds(result.groups.node_sets.at("lowy"),  {0, 1, 4, 5}));
    assert(sameIds(result.groups.node_sets.at("highy"), {2, 3, 6, 7}));
    assert(sameIds(result.groups.node_sets.at("lowz"),  {0, 1, 2, 3}));
    assert(sameIds(result.groups.node_sets.at("highz"), {4, 5, 6, 7}));

    result.mesh.validate();
    return 0;
}
