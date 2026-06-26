#include <cassert>
#include <cmath>

#include "feast/linalg/Vector.hpp"

int main()
{
    feast::Vector v(3);

    assert(v.size() == 3);

    v.setConstant(2.5);
    assert(std::abs(v[0] - 2.5) < 1e-12);
    assert(std::abs(v[1] - 2.5) < 1e-12);
    assert(std::abs(v[2] - 2.5) < 1e-12);

    v.setZero();
    assert(std::abs(v[0]) < 1e-12);
    assert(std::abs(v[1]) < 1e-12);
    assert(std::abs(v[2]) < 1e-12);

    v.resize(5);
    assert(v.size() == 5);

    v[3] = 7.0;
    v[4] = -1.5;
    assert(std::abs(v[3] - 7.0) < 1e-12);
    assert(std::abs(v[4] + 1.5) < 1e-12);

    return 0;
}
