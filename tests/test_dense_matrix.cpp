#include <cassert>
#include <cmath>

#include "feast/linalg/DenseMatrix.hpp"

namespace {
bool almostEqual(double a, double b, double eps = 1e-12)
{
    return std::abs(a - b) < eps;
}
}

int main()
{
    feast::DenseMatrix A(2, 2);
    A.setZero();

    A(0, 0) = 1.0;
    A(0, 1) = 2.0;
    A(1, 0) = 3.0;
    A(1, 1) = 4.0;

    assert(A.rows() == 2);
    assert(A.cols() == 2);
    assert(almostEqual(A(0, 0), 1.0));
    assert(almostEqual(A(1, 1), 4.0));

    feast::DenseMatrix At = A.transpose();
    assert(At.rows() == 2);
    assert(At.cols() == 2);
    assert(almostEqual(At(0, 1), 3.0));
    assert(almostEqual(At(1, 0), 2.0));

    feast::DenseMatrix I(2, 2);
    I.setIdentity();

    feast::DenseMatrix AI = A * I;
    assert(almostEqual(AI(0, 0), 1.0));
    assert(almostEqual(AI(0, 1), 2.0));
    assert(almostEqual(AI(1, 0), 3.0));
    assert(almostEqual(AI(1, 1), 4.0));

    feast::DenseMatrix B(2, 2);
    B.setConstant(2.0);
    B *= 0.5;
    assert(almostEqual(B(0, 0), 1.0));
    assert(almostEqual(B(1, 1), 1.0));

    feast::DenseMatrix C = A;
    C += I;
    assert(almostEqual(C(0, 0), 2.0));
    assert(almostEqual(C(1, 1), 5.0));

    return 0;
}
