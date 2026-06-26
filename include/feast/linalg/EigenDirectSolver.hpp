#pragma once

#include "feast/solvers/LinearSolver.hpp"

namespace feast {

class EigenDirectSolver final : public LinearSolver
{
public:
    EigenDirectSolver() = default;
    ~EigenDirectSolver() override = default;

    Vector solve(const SparseMatrix& K, const Vector& f) const override;
};

} // namespace feast
