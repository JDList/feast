#include "feast/kernel/Kernel.hpp"

#include <stdexcept>
//#include <chrono>
//#include <iostream>


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
    //const auto assemblyStart = std::chrono::steady_clock::now();

    GlobalAssembler assembler(dofMap.numDofs());

    AssemblyResult assembly = assembler.assembleLinearSystem(mesh,
                                                             dofMap,
                                                             boundaryConditions,
                                                             elementStiffnesses,
                                                             elementVectors);

    SparseMatrix K = assembly.stiffness;
    Vector f = assembly.force;
    
    //const auto assemblyEnd = std::chrono::steady_clock::now();
    //const auto constraintStart = std::chrono::steady_clock::now();
    

    ConstraintApplier::applyDirichlet(K, f, dofMap, boundaryConditions);

    //const auto constraintEnd = std::chrono::steady_clock::now();

    //const auto solverStart = std::chrono::steady_clock::now();

    Vector u = m_solver.solve(K, f);

    //const auto solverEnd = std::chrono::steady_clock::now();
    //const auto resultStart = std::chrono::steady_clock::now();
;
    LinearStaticResult result;
    result.stiffness = K;
    result.force = f;
    result.solution = u;

    //const auto resultEnd = std::chrono::steady_clock::now();

    //std::cout
    //    << "Assembly: " 
    //    << std::chrono::duration<double>(assemblyEnd - assemblyStart).count() << " s\n"
    //    << "Constraints: " 
    //    << std::chrono::duration<double>(constraintEnd - constraintStart).count() << " s\n"
    //    << "Solver: " 
    //    << std::chrono::duration<double>(solverEnd - solverStart).count() << " s\n"
    //    << "Results: " 
    //    << std::chrono::duration<double>(resultEnd - resultStart).count() << " s\n";
    return result;
}

} 
