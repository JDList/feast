#include "feast/geometry/builders/CuboidBuilder.hpp"

#include <stdexcept>

namespace feast
{

Geometry CuboidBuilder::build(const Cuboid& cuboid) const
{
    const double lx = cuboid.lengthX();
    const double ly = cuboid.lengthY();
    const double lz = cuboid.lengthZ();

    if (lx <= 0.0 || ly <= 0.0 || lz <= 0.0) {
        throw std::invalid_argument("Cuboid dimensions must be positive.");
    }

    Geometry geometry;

    // Vertices are ordered consistently so faces can refer to them by index.
    //
    //     z
    //     ^
    //     |
    //     4 ---- 5
    //    /|     /|
    //    7 ---- 6 |
    //    | 0 --|- 1 -> x
    //    |/    |/
    //    3 ---- 2
    //   /
    //  y

    const std::size_t v0 = geometry.addVertex({0.0, 0.0, 0.0});
    const std::size_t v1 = geometry.addVertex({lx,   0.0, 0.0});
    const std::size_t v2 = geometry.addVertex({lx,   ly,   0.0});
    const std::size_t v3 = geometry.addVertex({0.0,   ly,   0.0});

    const std::size_t v4 = geometry.addVertex({0.0, 0.0, lz});
    const std::size_t v5 = geometry.addVertex({lx,   0.0, lz});
    const std::size_t v6 = geometry.addVertex({lx,   ly,   lz});
    const std::size_t v7 = geometry.addVertex({0.0,   ly,   lz});

    geometry.addSurface(
        Surface{
            "lowx",
            FaceTag::LowX,
            {v0, v3, v7, v4}
        });

    geometry.addSurface(
        Surface{
            "highx",
            FaceTag::HighX,
            {v1, v2, v6, v5}
        });

    geometry.addSurface(
        Surface{
            "lowy",
            FaceTag::LowY,
            {v0, v1, v5, v4}
        });

    geometry.addSurface(
        Surface{
            "highy",
            FaceTag::HighY,
            {v3, v2, v6, v7}
        });

    geometry.addSurface(
        Surface{
            "lowz",
            FaceTag::LowZ,
            {v0, v1, v2, v3}
        });

    geometry.addSurface(
        Surface{
            "highz",
            FaceTag::HighZ,
            {v4, v5, v6, v7}
        });

    geometry.validate();
    return geometry;
}

} // namespace feast
