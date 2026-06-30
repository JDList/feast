#include "feast/bcs/BoundaryConditionResolver.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace feast {
namespace {

const std::vector<std::size_t>& lookupNodeSet(
    const MeshBuildResult& buildResult,
    const std::string& regionName)
{
    const auto it = buildResult.groups.node_sets.find(regionName);
    if (it == buildResult.groups.node_sets.end()) {
        throw std::out_of_range(
            "BoundaryConditionResolver: unknown node region name: " + regionName);
    }
    return it->second;
}

const std::vector<BoundaryFace>& lookupFaceSet(
    const MeshBuildResult& buildResult,
    const std::string& regionName)
{
    const auto it = buildResult.groups.face_sets.find(regionName);
    if (it == buildResult.groups.face_sets.end()) {
        throw std::out_of_range(
            "BoundaryConditionResolver: unknown face region name: " + regionName);
    }
    return it->second;
}

double triangleArea(const Node& a, const Node& b, const Node& c)
{
    const double ux = b.x - a.x;
    const double uy = b.y - a.y;
    const double uz = b.z - a.z;

    const double vx = c.x - a.x;
    const double vy = c.y - a.y;
    const double vz = c.z - a.z;

    const double cx = uy * vz - uz * vy;
    const double cy = uz * vx - ux * vz;
    const double cz = ux * vy - uy * vx;

    return 0.5 * std::sqrt(cx * cx + cy * cy + cz * cz);
}

double faceArea(const Mesh& mesh, const BoundaryFace& face)
{
    if (face.node_ids.size() < 3) {
        throw std::invalid_argument("BoundaryConditionResolver: face has fewer than 3 nodes.");
    }

    const auto& nodes = mesh.nodes();
    const Node& p0 = nodes.at(face.node_ids[0]);

    double area = 0.0;
    for (std::size_t i = 1; i + 1 < face.node_ids.size(); ++i) {
        const Node& p1 = nodes.at(face.node_ids[i]);
        const Node& p2 = nodes.at(face.node_ids[i + 1]);
        area += triangleArea(p0, p1, p2);
    }

    return area;
}

} // namespace

BoundaryConditionSet BoundaryConditionResolver::resolve(
    const RegionBoundaryConditionSet& regionConditions,
    const MeshBuildResult& buildResult)
{
    BoundaryConditionSet resolved;

    const BoundaryConditionSet dirichlet =
        resolveDirichlet(regionConditions, buildResult);

    const BoundaryConditionSet neumann =
        resolveNeumann(regionConditions, buildResult);

    for (const auto& bc : dirichlet.dirichletConditions()) {
        resolved.addDirichlet(bc.nodeId, bc.dof, bc.value);
    }

    for (const auto& bc : neumann.neumannConditions()) {
        resolved.addNeumann(bc.nodeId, bc.dof, bc.value);
    }

    return resolved;
}

BoundaryConditionSet BoundaryConditionResolver::resolveDirichlet(
    const RegionBoundaryConditionSet& regionConditions,
    const MeshBuildResult& buildResult)
{
    BoundaryConditionSet resolved;

    for (const auto& bc : regionConditions.regionDirichletConditions()) {
        const auto& nodeIds = lookupNodeSet(buildResult, bc.regionName);
        for (std::size_t nodeId : nodeIds) {
            resolved.addDirichlet(nodeId, bc.dof, bc.value);
        }
    }

    return resolved;
}

BoundaryConditionSet BoundaryConditionResolver::resolveNeumann(
    const RegionBoundaryConditionSet& regionConditions,
    const MeshBuildResult& buildResult)
{
    BoundaryConditionSet resolved;

    for (const auto& bc : regionConditions.regionNeumannConditions()) {
        const auto& faces = lookupFaceSet(buildResult, bc.regionName);

        std::unordered_map<std::size_t, double> nodeFactors;

        for (const auto& face : faces) {
            const double area = faceArea(buildResult.mesh, face);
            if (area <= 0.0) {
                continue;
            }

            const double share = area / static_cast<double>(face.node_ids.size());
            for (std::size_t nodeId : face.node_ids) {
                nodeFactors[nodeId] += share;
            }
        }

        if (nodeFactors.empty()) {
            throw std::runtime_error(
                "BoundaryConditionResolver: no usable boundary faces found for region: " +
                bc.regionName);
        }

        for (const auto& [nodeId, factor] : nodeFactors) {
            resolved.addNeumann(nodeId, bc.dof, bc.value * factor);
        }
    }

    return resolved;
}

} // namespace feast
