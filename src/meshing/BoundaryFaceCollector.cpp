#include "feast/meshing/BoundaryFaceCollector.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace feast
{
namespace
{

struct FaceRecord
{
    std::vector<std::size_t> node_ids;
    std::size_t count{0};
};

std::string faceKey(const std::vector<std::size_t>& node_ids)
{
    std::vector<std::size_t> sorted = node_ids;
    std::sort(sorted.begin(), sorted.end());

    std::string key;
    for (std::size_t i = 0; i < sorted.size(); ++i) {
        key += std::to_string(sorted[i]);
        if (i + 1 < sorted.size()) {
            key += '|';
        }
    }
    return key;
}

bool containsAll(
    const std::unordered_set<std::size_t>& allowed,
    const std::vector<std::size_t>& face_nodes)
{
    for (std::size_t node_id : face_nodes) {
        if (allowed.find(node_id) == allowed.end()) {
            return false;
        }
    }
    return true;
}

std::unordered_set<std::size_t> toSet(const std::vector<std::size_t>& values)
{
    return std::unordered_set<std::size_t>(values.begin(), values.end());
}

std::vector<std::vector<std::size_t>> elementFaces(const Element& element)
{
    if (element.type != ElementType::Tet4) {
        throw std::invalid_argument(
            "BoundaryFaceCollector currently supports Tet4 only. "
            "Add a face decomposition for the new element type here.");
    }

    if (element.node_ids.size() != 4) {
        throw std::invalid_argument("Tet4 element must have exactly 4 node ids.");
    }

    const auto& n = element.node_ids;

    return {
        { n[0], n[1], n[2] },
        { n[0], n[1], n[3] },
        { n[0], n[2], n[3] },
        { n[1], n[2], n[3] }
    };
}

} // namespace

void BoundaryFaceCollector::collect(MeshGroups& groups, const Mesh& mesh)
{
    groups.face_sets.clear();

    for (const auto& [region_name, _] : groups.node_sets) {
        groups.face_sets.emplace(region_name, std::vector<BoundaryFace>{});
    }

    std::unordered_map<std::string, FaceRecord> face_map;
    face_map.reserve(mesh.elements().size() * 4);

    for (const auto& element : mesh.elements()) {
        for (const auto& face_nodes : elementFaces(element)) {
            const std::string key = faceKey(face_nodes);

            auto it = face_map.find(key);
            if (it == face_map.end()) {
                face_map.emplace(key, FaceRecord{face_nodes, 1});
            } else {
                ++it->second.count;
            }
        }
    }

    // Keep only boundary faces (faces that appear once).
    for (const auto& [_, record] : face_map) {
        if (record.count != 1) {
            continue;
        }

        std::vector<std::string> matches;
        matches.reserve(groups.node_sets.size());

        for (const auto& [region_name, node_ids] : groups.node_sets) {
            const auto allowed = toSet(node_ids);
            if (containsAll(allowed, record.node_ids)) {
                matches.push_back(region_name);
            }
        }

        if (matches.size() != 1) {
            throw std::runtime_error(
                "BoundaryFaceCollector: unable to assign a boundary face uniquely to a region.");
        }

        groups.face_sets[matches.front()].push_back(BoundaryFace{record.node_ids});
    }
}

} // namespace feast
