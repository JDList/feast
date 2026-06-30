#include <algorithm>
#include <cassert>
#include <string>

#include "feast/geometry/builders/CuboidBuilder.hpp"
#include "feast/geometry/primitives/Cuboid.hpp"

static bool hasSurfaceName(const feast::Geometry& geometry, const std::string& name)
{
    return std::any_of(
        geometry.surfaces().begin(),
        geometry.surfaces().end(),
        [&](const feast::Surface& s) { return s.name == name; });
}

int main()
{
    feast::Cuboid cuboid(2.0, 3.0, 4.0);
    feast::CuboidBuilder builder;

    feast::Geometry geometry = builder.build(cuboid);

    assert(geometry.vertices().size() == 8);
    assert(geometry.surfaces().size() == 6);

    assert(hasSurfaceName(geometry, "lowx"));
    assert(hasSurfaceName(geometry, "highx"));
    assert(hasSurfaceName(geometry, "lowy"));
    assert(hasSurfaceName(geometry, "highy"));
    assert(hasSurfaceName(geometry, "lowz"));
    assert(hasSurfaceName(geometry, "highz"));

    geometry.validate();
    return 0;
}
