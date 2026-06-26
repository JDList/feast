#include <cassert>
#include <cmath>
#include <vector>


#include "feast/assembly/ConstraintApplier.hpp"
#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/linalg/Triplet.hpp"

int main()
{
    feast::SparseMatrix K;
    K.resize(2, 2);
    K.setZero();

    std::vector<feast::Triplet> triplets;
    triplets.emplace_back(0, 0, 2.0);
    triplets.emplace_back(0, 1, 1.0);
    triplets.emplace_back(1, 0, 1.0);
    triplets.emplace_back(1, 1, 2.0);
    K.setFromTriplets(triplets);

    feast::Vector f;
    f.resize(2);
    f[0] = 3.0;
    f[1] = 3.0;

    feast::DofMap dofMap;
    dofMap.resize(1, 2);

    feast::BoundaryConditionSet bcs;
    bcs.addDirichlet(0, 0, 1.0);

    feast::ConstraintApplier::applyDirichlet(K, f, dofMap, bcs);

    const auto& A = K.eigen();

    assert(A.rows() == 2);
    assert(A.cols() == 2);

    assert(std::abs(A.coeff(0, 0) - 1.0) < 1e-12);
    assert(std::abs(A.coeff(0, 1) - 0.0) < 1e-12);
    assert(std::abs(A.coeff(1, 0) - 0.0) < 1e-12);
    assert(std::abs(A.coeff(1, 1) - 2.0) < 1e-12);

    assert(std::abs(f[0] - 1.0) < 1e-12);
    assert(std::abs(f[1] - 2.0) < 1e-12);

    return 0;
}
