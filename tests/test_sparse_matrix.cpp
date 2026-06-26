#include <cassert>
#include <cmath>
#include <vector>


#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Triplet.hpp"

namespace {
bool almostEqual(double a, double b, double eps = 1e-12)
{
    return std::abs(a - b) < eps;
}
}

int main()
{
    feast::SparseMatrix K(3, 3);
    K.reserve(4);

    std::vector<feast::Triplet> triplets;
    triplets.emplace_back(0, 0, 10.0);
    triplets.emplace_back(0, 1, -2.0);
    triplets.emplace_back(1, 0, -2.0);
    triplets.emplace_back(1, 1, 5.0);

    K.setFromTriplets(triplets);

    assert(K.rows() == 3);
    assert(K.cols() == 3);

    const auto& Ek = K.eigen();
    assert(almostEqual(Ek.coeff(0, 0), 10.0));
    assert(almostEqual(Ek.coeff(0, 1), -2.0));
    assert(almostEqual(Ek.coeff(1, 0), -2.0));
    assert(almostEqual(Ek.coeff(1, 1), 5.0));
    assert(almostEqual(Ek.coeff(2, 2), 0.0));

    K.setZero();
    assert(K.rows() == 3);
    assert(K.cols() == 3);

    return 0;
}
