#include "feast/materials/LinearElastic.hpp"

#include <stdexcept>

namespace feast {

LinearElastic::LinearElastic(
    double youngs_modulus,
    double poisson_ratio,
    double density)
    : E_(youngs_modulus),
      nu_(poisson_ratio),
      rho_(density)
{
    if (E_ <= 0.0) {
        throw std::invalid_argument("Young's modulus must be positive.");
    }

    if (nu_ <= -1.0 || nu_ >= 0.5) {
        throw std::invalid_argument("Poisson ratio must be in (-1, 0.5).");
    }

    if (rho_ < 0.0) {
        throw std::invalid_argument("Density must be non-negative.");
    }
}

MaterialType LinearElastic::type() const
{
    return MaterialType::LinearElastic;
}

double LinearElastic::youngsModulus() const
{
    return E_;
}

double LinearElastic::poissonRatio() const
{
    return nu_;
}

double LinearElastic::density() const
{
    return rho_;
}

}
