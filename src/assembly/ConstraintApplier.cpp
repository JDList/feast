#include "feast/assembly/ConstraintApplier.hpp"

#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace feast {

void ConstraintApplier::applyDirichlet(SparseMatrix& K,
                                       Vector& f,
                                       const DofMap& dofMap,
                                       const BoundaryConditionSet& boundaryConditions)
{
    if (static_cast<std::size_t>(K.rows()) != f.size())
        throw std::invalid_argument("ConstraintApplier: matrix and vector size mismatch.");

    if (K.rows() != K.cols())
        throw std::invalid_argument("ConstraintApplier: stiffness matrix must be square");

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

    if (constrainedDofs.empty())
    {
        return;
    }

    const std::size_t systemSize = K.rows();

    std::vector<unsigned char> isConstrained(systemSize, 0);

    std::vector<double> prescribedValues(systemSize,0.0);

    for (const auto& [globalDof, value] : constrainedDofs)
    {
        isConstrained[globalDof] = 1;
        prescribedValues[globalDof] = value;
    }

    std::vector<Triplet> entries;
    entries.reserve(K.nonZeros() + constrainedDofs.size());

    K.forEachNonZero(
        [&](std::size_t row,
            std::size_t col,
            double matrixValue)
        {
            // Remove every entry belonging to a constrained row.
            if (isConstrained[row])
            {
                return;
            }

            // Remove constrained columns and move their contribution
            // onto the right-hand side.
            if (isConstrained[col])
            {
                f[row] -=
                    matrixValue * prescribedValues[col];

                return;
            }

            entries.push_back(
                Triplet{row, col, matrixValue});
        });

    // Replace each constrained equation with:
    //
    // u_g = prescribedValue
    for (const auto& [globalDof, value] : constrainedDofs)
    {
        entries.push_back(
            Triplet{globalDof, globalDof, 1.0});

        f[globalDof] = value;
    }

    K.setFromTriplets(entries);}

} // namespace feast
