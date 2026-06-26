#include "feast/linalg/EigenDirectSolver.hpp"

#include <stdexcept>

#include <Eigen/SparseCholesky>

namespace feast {

Vector EigenDirectSolver::solve(const SparseMatrix& K, const Vector& f) const
{
    const auto& Ke = K.eigen();
    const auto& fe = f.eigen();

    if (Ke.rows() != Ke.cols())
        throw std::invalid_argument("EigenDirectSolver: stiffness matrix must be square.");

    if (Ke.rows() != fe.size())
        throw std::invalid_argument("EigenDirectSolver: matrix/vector size mismatch.");

    // For structural mechanics with Dirichlet constraints applied,
    // the modified system is typically symmetric positive definite.
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(Ke);

    if (solver.info() != Eigen::Success)
        throw std::runtime_error("EigenDirectSolver: factorization failed.");

    Eigen::VectorXd x = solver.solve(fe);

    if (solver.info() != Eigen::Success)
        throw std::runtime_error("EigenDirectSolver: solve failed.");

    Vector result;
    result.resize(static_cast<std::size_t>(x.size()));

    for (Eigen::Index i = 0; i < x.size(); ++i)
        result[static_cast<std::size_t>(i)] = x[i];

    return result;
}

} // namespace feast
