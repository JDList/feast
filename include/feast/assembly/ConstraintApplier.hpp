#pragma once

#include <cstddef>

#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"

namespace feast {

class ConstraintApplier
{
public:
    ConstraintApplier() = delete;

    // Enforces Dirichlet conditions in-place on the linear system:
    //     K u = f
    // 
    // Uses the standard row/column modification approach:
    //   - subtract constrained value contributions from the RHS
    //   - zero the constrained row and column
    //   - set diagonal entry to 1
    //   - set RHS entry to the prescribed value
    static void applyDirichlet(SparseMatrix& K,
                               Vector& f,
                               const DofMap& dofMap,
                               const BoundaryConditionSet& boundaryConditions);
};

} // namespace feast
