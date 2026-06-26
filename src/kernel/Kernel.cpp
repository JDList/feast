#include "feast/kernel/Kernel.hpp"

#include <stdexcept>

namespace feast {

Kernel::Kernel(const LinearSolver& solver)
    : m_solver(solver)
{
}

LinearStaticResult Kernel::solveLinearStatic(const Mesh& mesh,
                                             const DofMap& dofMap,
                                             const BoundaryConditionSet& boundaryConditions,
                                             const std::vector<DenseMatrix>& elementStiffnesses,
                                             const std::vector<Vector>& elementVectors) const
{
    GlobalAssembler assembler(dofMap.numDofs());

    AssemblyResult assembly = assembler.assembleLinearSystem(mesh,
                                                             dofMap,
                                                             boundaryConditions,
                                                             elementStiffnesses,
                                                             elementVectors);

    SparseMatrix K = assembly.stiffness;
    Vector f = assembly.force;

    ConstraintApplier::applyDirichlet(K, f, dofMap, boundaryConditions);

    Vector u = m_solver.solve(K, f);

    LinearStaticResult result;
    result.stiffness = K;
    result.force = f;
    result.solution = u;
    return result;
}

} // namespace feast
