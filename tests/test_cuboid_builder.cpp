#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "feast/geometry/builders/CuboidBuilder.hpp"
#include "feast/geometry/primitives/Cuboid.hpp"

static const feast::Surface* findSurface(const feast::Geometry& geometry, const std::string& name)
{
    auto it = std::find_if(
        geometry.surfaces().begin(),
        geometry.surfaces().end(),
        [&](const feast::Surface& s) { return s.name == name; });

    if (it == geometry.surfaces().end()) {
        return nullptr;
    }

    return &(*it);
}

static bool sameIds(std::vector<std::size_t> a, std::vector<std::size_t> b)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

int main()
{
    feast::Cuboid cuboid(1.0, 2.0, 3.0);
    feast::CuboidBuilder builder;

    feast::Geometry geometry = builder.build(cuboid);

    assert(geometry.vertices().size() == 8);
    assert(geometry.surfaces().size() == 6);

    const feast::Surface* lowx = findSurface(geometry, "lowx");
    const feast::Surface* highz = findSurface(geometry, "highz");

    assert(lowx != nullptr);
    assert(highz != nullptr);

    assert(lowx->tag == feast::FaceTag::LowX);
    assert(highz->tag == feast::FaceTag::HighZ);

    assert(lowx->vertex_ids.size() == 4);
    assert(highz->vertex_ids.size() == 4);

    assert(sameIds(lowx->vertex_ids, {0, 3, 7, 4}));
    assert(sameIds(highz->vertex_ids, {4, 5, 6, 7}));

    geometry.validate();
    return 0;
}
