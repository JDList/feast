#include "feast/geometry/primitives/Cuboid.hpp"

#include <stdexcept>

namespace feast
{

Cuboid::Cuboid(double lx, double ly, double lz)
    : lx_(lx),
      ly_(ly),
      lz_(lz)
{
    if (lx_ <= 0.0 || ly_ <= 0.0 || lz_ <= 0.0) {
        throw std::invalid_argument("Cuboid dimensions must be positive.");
    }
}

double Cuboid::lengthX() const { return lx_; }
double Cuboid::lengthY() const { return ly_; }
double Cuboid::lengthZ() const { return lz_; }

} // namespace feast
