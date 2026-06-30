#include "feast/meshing/StructuredTetMesher.hpp"
#include "feast/meshing/BoundaryFaceCollector.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace feast
{
namespace
{

std::size_t divisionsFromLength(double length, double target_element_size)
{
    if (length <= 0.0) {
        throw std::invalid_argument("Geometry dimensions must be positive.");
    }

    if (target_element_size <= 0.0) {
        throw std::invalid_argument("Target element size must be positive.");
    }

    const double raw = length / target_element_size;
    const std::size_t n = static_cast<std::size_t>(std::ceil(raw));

    return std::max<std::size_t>(1, n);
}

std::size_t nodeIndex(
    std::size_t i,
    std::size_t j,
    std::size_t k,
    std::size_t nx,
    std::size_t ny)
{
    return i + (nx + 1) * (j + (ny + 1) * k);
}

void addUnique(std::vector<std::size_t>& values, std::size_t value)
{
    if (std::find(values.begin(), values.end(), value) == values.end()) {
        values.push_back(value);
    }
}

} // namespace

StructuredTetMesher::StructuredTetMesher(double target_element_size)
    : target_element_size_(target_element_size)
{
    if (target_element_size_ <= 0.0) {
        throw std::invalid_argument("Target element size must be positive.");
    }
}

double StructuredTetMesher::targetElementSize() const
{
    return target_element_size_;
}

MeshBuildResult StructuredTetMesher::generate(const Geometry& geometry) const
{
    geometry.validate();

    if (geometry.vertices().size() != 8) {
        throw std::invalid_argument(
            "StructuredTetMesher currently requires exactly 8 vertices.");
    }

    if (geometry.surfaces().size() != 6) {
        throw std::invalid_argument(
            "StructuredTetMesher currently requires exactly 6 surfaces.");
    }

    double min_x = geometry.vertices().front().x;
    double max_x = geometry.vertices().front().x;
    double min_y = geometry.vertices().front().y;
    double max_y = geometry.vertices().front().y;
    double min_z = geometry.vertices().front().z;
    double max_z = geometry.vertices().front().z;

    for (const auto& p : geometry.vertices()) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);

        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);

        min_z = std::min(min_z, p.z);
        max_z = std::max(max_z, p.z);
    }

    const double lx = max_x - min_x;
    const double ly = max_y - min_y;
    const double lz = max_z - min_z;

    const std::size_t nx = divisionsFromLength(lx, target_element_size_);
    const std::size_t ny = divisionsFromLength(ly, target_element_size_);
    const std::size_t nz = divisionsFromLength(lz, target_element_size_);

    MeshBuildResult result;

    result.groups.node_sets["lowx"] = {};
    result.groups.node_sets["highx"] = {};
    result.groups.node_sets["lowy"] = {};
    result.groups.node_sets["highy"] = {};
    result.groups.node_sets["lowz"] = {};
    result.groups.node_sets["highz"] = {};

    const double dx = lx / static_cast<double>(nx);
    const double dy = ly / static_cast<double>(ny);
    const double dz = lz / static_cast<double>(nz);

    std::size_t node_id = 0;

    for (std::size_t k = 0; k <= nz; ++k) {
        const double z = min_z + dz * static_cast<double>(k);

        for (std::size_t j = 0; j <= ny; ++j) {
            const double y = min_y + dy * static_cast<double>(j);

            for (std::size_t i = 0; i <= nx; ++i) {
                const double x = min_x + dx * static_cast<double>(i);

                Node node{};
                node.id = node_id;
                node.x = x;
                node.y = y;
                node.z = z;

                result.mesh.addNode(node);

                if (i == 0) {
                    addUnique(result.groups.node_sets["lowx"], node_id);
                }
                if (i == nx) {
                    addUnique(result.groups.node_sets["highx"], node_id);
                }
                if (j == 0) {
                    addUnique(result.groups.node_sets["lowy"], node_id);
                }
                if (j == ny) {
                    addUnique(result.groups.node_sets["highy"], node_id);
                }
                if (k == 0) {
                    addUnique(result.groups.node_sets["lowz"], node_id);
                }
                if (k == nz) {
                    addUnique(result.groups.node_sets["highz"], node_id);
                }

                ++node_id;
            }
        }
    }

    std::size_t element_id = 0;

    for (std::size_t k = 0; k < nz; ++k) {
        for (std::size_t j = 0; j < ny; ++j) {
            for (std::size_t i = 0; i < nx; ++i) {
                const std::size_t n000 = nodeIndex(i,     j,     k,     nx, ny);
                const std::size_t n100 = nodeIndex(i + 1, j,     k,     nx, ny);
                const std::size_t n110 = nodeIndex(i + 1, j + 1, k,     nx, ny);
                const std::size_t n010 = nodeIndex(i,     j + 1, k,     nx, ny);
                const std::size_t n001 = nodeIndex(i,     j,     k + 1, nx, ny);
                const std::size_t n101 = nodeIndex(i + 1, j,     k + 1, nx, ny);
                const std::size_t n111 = nodeIndex(i + 1, j + 1, k + 1, nx, ny);
                const std::size_t n011 = nodeIndex(i,     j + 1, k + 1, nx, ny);

                auto addTet = [&](std::size_t a,
                                  std::size_t b,
                                  std::size_t c,
                                  std::size_t d)
                {
                    Element element{};
                    element.id = element_id++;
                    element.type = ElementType::Tet4;
                    element.node_ids = {a, b, c, d};
                    element.material_id = 0;
                    result.mesh.addElement(element);
                };

                addTet(n000, n100, n110, n111);
                addTet(n000, n110, n010, n111);
                addTet(n000, n010, n011, n111);
                addTet(n000, n011, n001, n111);
                addTet(n000, n001, n101, n111);
                addTet(n000, n101, n100, n111);
            }
        }
    }

    BoundaryFaceCollector::collect(result.groups, result.mesh);

    return result;
}

} // namespace feast
