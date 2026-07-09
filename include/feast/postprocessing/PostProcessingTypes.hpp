#pragma once

#include "feast/linalg/Vector.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace feast {

using Vector3 = std::array<double, 3>;
using Voigt6 = std::array<double, 6>;
using Principal3 = std::array<double, 3>;

struct ScalarSummary
{
    std::size_t count{0};
    double sum{0.0};
    double mean{0.0};
    double min{0.0};
    double max{0.0};
    double range{0.0};
    double standardDeviation{0.0};
};

ScalarSummary summarizeValues(const std::vector<double>& values);

struct ScalarFieldData
{
    std::vector<double> values;
    ScalarSummary summary;
};

struct VectorFieldData
{
    std::vector<Vector3> values;

    ScalarFieldData x;
    ScalarFieldData y;
    ScalarFieldData z;
    ScalarFieldData magnitude;
};

struct TensorFieldData
{
    std::vector<Voigt6> values;

    ScalarFieldData xx;
    ScalarFieldData yy;
    ScalarFieldData zz;
    ScalarFieldData xy;
    ScalarFieldData yz;
    ScalarFieldData zx;
    ScalarFieldData magnitude;
};

struct PrincipalFieldData
{
    std::vector<Principal3> values;

    ScalarFieldData minimum;
    ScalarFieldData middle;
    ScalarFieldData maximum;
};

struct StressFieldData
{
    std::vector<Voigt6> values;

    ScalarFieldData xx;
    ScalarFieldData yy;
    ScalarFieldData zz;
    ScalarFieldData xy;
    ScalarFieldData yz;
    ScalarFieldData zx;
    ScalarFieldData magnitude;

    ScalarFieldData hydrostatic;
    ScalarFieldData vonMises;

    PrincipalFieldData principal;
};

struct PostProcessResult
{
    VectorFieldData displacement;
    TensorFieldData strain;
    StressFieldData stress;
};

} // namespace feast
