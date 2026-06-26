#include <cassert>
#include <cmath>
#include <vector>

#include <Eigen/SparseCore>

#include "feast/assembly/GlobalAssembler.hpp"
#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/linalg/DenseMatrix.hpp"
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
    bcs.addNeumann(1, 2, 5.0);

    feast::DenseMatrix Ke;
    Ke.resize(12, 12);
    Ke.setZero();
    Ke.setIdentity();

    feast::Vector fe;
    fe.resize(12);
    fe.setConstant(1.0);

    std::vector<feast::DenseMatrix> elementStiffnesses = {Ke};
    std::vector<feast::Vector> elementVectors = {fe};

    feast::GlobalAssembler assembler(dofMap.numDofs());
    const auto result = assembler.assembleLinearSystem(mesh,
                                                      dofMap,
                                                      bcs,
                                                      elementStiffnesses,
                                                      elementVectors);

    assert(result.force.size() == 12);

    for (std::size_t i = 0; i < 12; ++i)
    {
        if (i == 5)
            assert(std::abs(result.force[i] - 6.0) < 1e-12);
        else
            assert(std::abs(result.force[i] - 1.0) < 1e-12);
    }

    const auto& K = result.stiffness.eigen();
    assert(K.rows() == 12);
    assert(K.cols() == 12);

    for (int i = 0; i < K.rows(); ++i)
    {
        for (int j = 0; j < K.cols(); ++j)
        {
            const double expected = (i == j) ? 1.0 : 0.0;
            assert(std::abs(K.coeff(i, j) - expected) < 1e-12);
        }
    }

    return 0;
}
