#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "feast/geometry/FaceTag.hpp"

namespace feast
{

struct Point3D
{
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

struct Surface
{
    std::string name;
    FaceTag tag;
    std::vector<std::size_t> vertex_ids;
};

class Geometry
{
public:
    std::size_t addVertex(const Point3D& point);
    void addSurface(const Surface& surface);

    const std::vector<Point3D>& vertices() const;
    const std::vector<Surface>& surfaces() const;

    void clear();
    void validate() const;

private:
    std::vector<Point3D> vertices_;
    std::vector<Surface> surfaces_;
};

} // namespace feast
