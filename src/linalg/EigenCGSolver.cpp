#include "feast/linalg/EigenCGSolver.hpp"

#include <stdexcept>

#include <Eigen/IterativeLinearSolvers>

namespace feast {

EigenCGSolver::EigenCGSolver(
    double tolerance,
    std::size_t maxIterations)
    : m_tolerance(tolerance),
      m_maxIterations(maxIterations)
{
    if (m_tolerance <= 0.0)
    {
        throw std::invalid_argument(
            "EigenCGSolver: tolerance must be greater than zero.");
    }

    if (m_maxIterations == 0)
    {
        throw std::invalid_argument(
            "EigenCGSolver: maximum iterations must be greater than zero.");
    }
}

Vector EigenCGSolver::solve(
    const SparseMatrix& K,
    const Vector& f) const
{
    const auto& Ke = K.eigen();
    const auto& fe = f.eigen();

    if (Ke.rows() != Ke.cols())
    {
        throw std::invalid_argument(
            "EigenCGSolver: stiffness matrix must be square.");
    }

    if (Ke.rows() != fe.size())
    {
        throw std::invalid_argument(
            "EigenCGSolver: matrix/vector size mismatch.");
    }

    using Solver = Eigen::ConjugateGradient<
        Eigen::SparseMatrix<double>,
        Eigen::Lower | Eigen::Upper,
        Eigen::DiagonalPreconditioner<double>>;

    Solver solver;

    solver.setTolerance(m_tolerance);
    solver.setMaxIterations(
        static_cast<Eigen::Index>(m_maxIterations));

    solver.compute(Ke);

    if (solver.info() != Eigen::Success)
    {
        m_converged = false;
        m_iterations = 0;
        m_estimatedError = 0.0;

        throw std::runtime_error(
            "EigenCGSolver: matrix setup failed.");
    }

    Eigen::VectorXd eigenSolution = solver.solve(fe);

    m_iterations =
        static_cast<std::size_t>(solver.iterations());

    m_estimatedError = solver.error();

    m_converged = solver.info() == Eigen::Success;

    if (!m_converged)
    {
        throw std::runtime_error(
            "EigenCGSolver: solver did not converge.");
    }

    Vector result;
    result.resize(
        static_cast<std::size_t>(eigenSolution.size()));

    for (Eigen::Index i = 0; i < eigenSolution.size(); ++i)
    {
        result[static_cast<std::size_t>(i)] =
            eigenSolution[i];
    }

    return result;
}

double EigenCGSolver::tolerance() const noexcept
{
    return m_tolerance;
}

std::size_t EigenCGSolver::maxIterations() const noexcept
{
    return m_maxIterations;
}

bool EigenCGSolver::converged() const noexcept
{
    return m_converged;
}

std::size_t EigenCGSolver::iterations() const noexcept
{
    return m_iterations;
}

double EigenCGSolver::estimatedError() const noexcept
{
    return m_estimatedError;
}

} // namespace feast
