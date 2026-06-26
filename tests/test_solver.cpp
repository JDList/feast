#include <cassert>
#include <cmath>
#include <vector>


#include "feast/linalg/EigenDirectSolver.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/linalg/Triplet.hpp"

int main()
{
    feast::SparseMatrix K;
    K.resize(3, 3);
    K.setZero();

    std::vector<feast::Triplet> triplets;
    triplets.emplace_back(0, 0, 4.0);
    triplets.emplace_back(1, 1, 9.0);
    triplets.emplace_back(2, 2, 16.0);
    K.setFromTriplets(triplets);

    feast::Vector f;
    f.resize(3);
    f[0] = 8.0;
    f[1] = 18.0;
    f[2] = 32.0;

    feast::EigenDirectSolver solver;
    feast::Vector u = solver.solve(K, f);

    assert(u.size() == 3);
    assert(std::abs(u[0] - 2.0) < 1e-12);
    assert(std::abs(u[1] - 2.0) < 1e-12);
    assert(std::abs(u[2] - 2.0) < 1e-12);

    return 0;
}
