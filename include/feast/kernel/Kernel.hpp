#pragma once

#include <vector>

#include "feast/assembly/ConstraintApplier.hpp"
#include "feast/assembly/GlobalAssembler.hpp"
#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/mesh/Mesh.hpp"
#include "feast/solvers/LinearSolver.hpp"

namespace feast {

struct LinearStaticResult
{
    SparseMatrix stiffness;
    Vector force;
    Vector solution;
};

class Kernel
{
public:
    explicit Kernel(const LinearSolver& solver);

    LinearStaticResult solveLinearStatic(const Mesh& mesh,
                                         const DofMap& dofMap,
                                         const BoundaryConditionSet& boundaryConditions,
                                         const std::vector<DenseMatrix>& elementStiffnesses,
                                         const std::vector<Vector>& elementVectors = {}) const;

private:
    const LinearSolver& m_solver;
};

} // namespace feast
