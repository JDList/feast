#include <cassert>
#include <cmath>
#include <vector>

#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/kernel/Kernel.hpp"
#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/EigenDirectSolver.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/mesh/Mesh.hpp"

int main()
{
    feast::Mesh mesh;
    mesh.addNode(feast::Node{0, 0.0, 0.0, 0.0});
    mesh.addNode(feast::Node{1, 1.0, 0.0, 0.0});
    mesh.addNode(feast::Node{2, 0.0, 1.0, 0.0});
    mesh.addNode(feast::Node{3, 0.0, 0.0, 1.0});

    feast::Element element;
    element.id = 0;
    element.type = feast::ElementType::Tet4;
    element.node_ids = {0, 1, 2, 3};
    element.material_id = 0;
    mesh.addElement(element);

    feast::DofMap dofMap;
    dofMap.resize(4, 3);

    feast::BoundaryConditionSet bcs;
    bcs.addDirichlet(0, 0, 0.0);
    bcs.addNeumann(1, 2, 4.0);

    feast::DenseMatrix Ke;
    Ke.resize(12, 12);
    Ke.setZero();
    Ke.setIdentity();

    std::vector<feast::DenseMatrix> elementStiffnesses = {Ke};

    feast::EigenDirectSolver solver;
    feast::Kernel kernel(solver);

    const auto result = kernel.solveLinearStatic(mesh,
                                                 dofMap,
                                                 bcs,
                                                 elementStiffnesses);

    assert(result.solution.size() == 12);
    assert(std::abs(result.solution[5] - 4.0) < 1e-12);
    assert(std::abs(result.solution[0] - 0.0) < 1e-12);
    assert(std::abs(result.solution[1] - 0.0) < 1e-12);
    assert(std::abs(result.solution[3] - 0.0) < 1e-12);

    return 0;
}
