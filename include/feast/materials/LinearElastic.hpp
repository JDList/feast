#pragma once

#include "feast/materials/Material.hpp"

namespace feast {

class LinearElastic : public Material {
public:
    LinearElastic() = default;
    LinearElastic(double youngs_modulus, double poisson_ratio, double density = 0.0);

    MaterialType type() const override;

    double youngsModulus() const;
    double poissonRatio() const;
    double density() const;

private:
    double E_ = 0.0;
    double nu_ = 0.0;
    double rho_ = 0.0;
};

}
