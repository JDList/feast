#include <cassert>
#include <cmath>
#include <stdexcept>

#include "feast/materials/LinearElastic.hpp"

namespace {
bool almostEqual(double a, double b, double eps = 1e-12)
{
    return std::abs(a - b) < eps;
}
}

int main()
{
    feast::LinearElastic steel(210e9, 0.30, 7850.0);

    assert(steel.type() == feast::MaterialType::LinearElastic);
    assert(almostEqual(steel.youngsModulus(), 210e9));
    assert(almostEqual(steel.poissonRatio(), 0.30));
    assert(almostEqual(steel.density(), 7850.0));

    bool threw = false;
    try {
        feast::LinearElastic bad_E(-1.0, 0.30, 1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        feast::LinearElastic bad_nu(1.0, 0.6, 1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        feast::LinearElastic bad_rho(1.0, 0.3, -1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    return 0;
}
