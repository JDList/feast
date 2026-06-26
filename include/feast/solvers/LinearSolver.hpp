#pragma once

#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"

namespace feast {

class LinearSolver
{
public:
    virtual ~LinearSolver() = default;

    virtual Vector solve(const SparseMatrix& K, const Vector& f) const = 0;
};

} // namespace feast
