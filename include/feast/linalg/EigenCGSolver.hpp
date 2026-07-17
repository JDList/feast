#pragma once

#include <cstddef>

#include "feast/solvers/LinearSolver.hpp"

namespace feast {

/**
 * Iterative conjugate-gradient solver for symmetric positive-definite systems.
 *
 * This implementation uses Eigen::ConjugateGradient with a diagonal
 * preconditioner. The constrained stiffness matrix must remain symmetric and
 * positive definite for the method to be valid.
 */
class EigenCGSolver final : public LinearSolver {
public:
    explicit EigenCGSolver(
        double tolerance = 1.0e-10,
        std::size_t maxIterations = 1000);

    Vector solve(const SparseMatrix& K, const Vector& f) const override;

    double tolerance() const noexcept;
    std::size_t maxIterations() const noexcept;

    bool converged() const noexcept;
    std::size_t iterations() const noexcept;
    double estimatedError() const noexcept;

private:
    double m_tolerance;
    std::size_t m_maxIterations;

    // solve(...) is const because LinearSolver requires it. These fields only
    // store diagnostics from the most recent solve.
    mutable bool m_converged{false};
    mutable std::size_t m_iterations{0};
    mutable double m_estimatedError{0.0};
};

} // namespace feast
