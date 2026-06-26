#include "feast/assembly/ConstraintApplier.hpp"

#include <stdexcept>
#include <unordered_map>

namespace feast {

void ConstraintApplier::applyDirichlet(SparseMatrix& K,
                                       Vector& f,
                                       const DofMap& dofMap,
                                       const BoundaryConditionSet& boundaryConditions)
{
    if (static_cast<std::size_t>(K.rows()) != f.size())
        throw std::invalid_argument("ConstraintApplier: matrix and vector size mismatch.");

    // Map global dof -> prescribed value.
    // This also lets us detect duplicate constraints on the same dof.
    std::unordered_map<std::size_t, double> constrainedDofs;
    constrainedDofs.reserve(boundaryConditions.numDirichlet());

    for (const auto& bc : boundaryConditions.dirichletConditions())
    {
        const int globalDofInt = dofMap.dof(bc.nodeId, bc.dof);
        if (globalDofInt < 0)
            throw std::out_of_range("ConstraintApplier: invalid global DOF from Dirichlet condition.");

        const std::size_t globalDof = static_cast<std::size_t>(globalDofInt);

        auto [it, inserted] = constrainedDofs.emplace(globalDof, bc.value);
        if (!inserted)
        {
            if (it->second != bc.value)
                throw std::invalid_argument("ConstraintApplier: conflicting Dirichlet values on the same DOF.");
        }
    }

    auto& A = K.eigen();

    for (const auto& [globalDof, value] : constrainedDofs)
    {
        if (globalDof >= f.size())
            throw std::out_of_range("ConstraintApplier: constrained DOF out of range.");

        // Adjust RHS of all unconstrained equations:
        // f_i <- f_i - A(i, g) * u_g
        for (std::size_t row = 0; row < static_cast<std::size_t>(A.rows()); ++row)
        {
            if (row == globalDof)
                continue;

            const double a_ig = A.coeff(static_cast<int>(row), static_cast<int>(globalDof));
            if (a_ig != 0.0)
                f[row] -= a_ig * value;
        }

        // Zero constrained row and column.
        for (std::size_t col = 0; col < static_cast<std::size_t>(A.cols()); ++col)
        {
            if (col == globalDof)
                continue;

            A.coeffRef(static_cast<int>(globalDof), static_cast<int>(col)) = 0.0;
        }

        for (std::size_t row = 0; row < static_cast<std::size_t>(A.rows()); ++row)
        {
            if (row == globalDof)
                continue;

            A.coeffRef(static_cast<int>(row), static_cast<int>(globalDof)) = 0.0;
        }

        A.coeffRef(static_cast<int>(globalDof), static_cast<int>(globalDof)) = 1.0;
        f[globalDof] = value;
    }

    A.makeCompressed();
}

} // namespace feast
