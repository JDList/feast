#include "feast/geometry/Geometry.hpp"

#include <set>
#include <stdexcept>

namespace feast
{

std::size_t Geometry::addVertex(const Point3D& point)
{
    vertices_.push_back(point);
    return vertices_.size() - 1;
}

void Geometry::addSurface(const Surface& surface)
{
    surfaces_.push_back(surface);
}

const std::vector<Point3D>& Geometry::vertices() const
{
    return vertices_;
}

const std::vector<Surface>& Geometry::surfaces() const
{
    return surfaces_;
}

void Geometry::clear()
{
    vertices_.clear();
    surfaces_.clear();
}

void Geometry::validate() const
{
    if (vertices_.empty()) {
        throw std::runtime_error("Geometry must contain at least one vertex.");
    }

    if (surfaces_.empty()) {
        throw std::runtime_error("Geometry must contain at least one surface.");
    }

    std::set<std::string> surface_names;
    std::set<int> surface_tags;

    for (const auto& surface : surfaces_) {
        if (surface.name.empty()) {
            throw std::invalid_argument("Geometry surface name cannot be empty.");
        }

        if (surface.vertex_ids.size() < 3) {
            throw std::invalid_argument(
                "Geometry surface must reference at least 3 vertices.");
        }

        if (!surface_names.insert(surface.name).second) {
            throw std::invalid_argument(
                "Geometry surface names must be unique.");
        }

        if (!surface_tags.insert(static_cast<int>(surface.tag)).second) {
            throw std::invalid_argument(
                "Geometry surface tags must be unique.");
        }

        for (std::size_t vertex_id : surface.vertex_ids) {
            if (vertex_id >= vertices_.size()) {
                throw std::out_of_range(
                    "Geometry surface references an invalid vertex index.");
            }
        }
    }
}

} // namespace feast
