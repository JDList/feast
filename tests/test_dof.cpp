#include <cassert>

#include "feast/dof/DofMap.hpp"
#include "feast/dof/DofManager.hpp"
#include "feast/mesh/Mesh.hpp"

int main()
{
    feast::DofMap map;
    map.resize(4, 3);

    assert(map.numNodes() == 4);
    assert(map.dofsPerNode() == 3);
    assert(map.numDofs() == 12);

    assert(map.dof(0, 0) == 0);
    assert(map.dof(0, 1) == 1);
    assert(map.dof(0, 2) == 2);
    assert(map.dof(1, 0) == 3);
    assert(map.dof(3, 2) == 11);

    std::vector<std::size_t> nodeIds = {0, 1, 2, 3};
    const auto elementDofs = map.elementDofs(nodeIds);

    assert(elementDofs.size() == 12);
    for (std::size_t i = 0; i < 12; ++i)
        assert(elementDofs[i] == static_cast<int>(i));

    feast::Mesh mesh;
    mesh.addNode(feast::Node{0, 0.0, 0.0, 0.0});
    mesh.addNode(feast::Node{1, 1.0, 0.0, 0.0});
    mesh.addNode(feast::Node{2, 0.0, 1.0, 0.0});
    mesh.addNode(feast::Node{3, 0.0, 0.0, 1.0});

    feast::DofManager manager(3);
    manager.build(mesh);

    assert(manager.numDofs() == 12);
    assert(manager.dofMap().numNodes() == 4);
    assert(manager.dofMap().dofsPerNode() == 3);
    assert(manager.dofMap().dof(2, 1) == 7);

    return 0;
}
