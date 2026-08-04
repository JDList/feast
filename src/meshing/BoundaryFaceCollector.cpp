#include "feast/meshing/BoundaryFaceCollector.hpp"

#include <array>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace feast
{
namespace
{

struct FaceKey
{
    std::array<std::size_t, 3> nodes;

    bool operator==(const FaceKey& other) const noexcept
    {
        return nodes == other.nodes;
    }
};

struct FaceKeyHash
{
    std::size_t operator()(const FaceKey& face) const noexcept
    {
        std::size_t seed = 0;

        for (const std::size_t value : face.nodes)
        {
            seed ^= std::hash<std::size_t>{}(value)
                + 0x9e3779b97f4a7c15ULL
                + (seed << 6)
                + (seed >> 2);
        }

        return seed;
    }
};

struct FaceRecord
{
    std::array<std::size_t, 3> node_ids;
    std::size_t count{1};
};

FaceKey makeFaceKey(
    std::size_t a,
    std::size_t b,
    std::size_t c) noexcept
{
    if (a > b)
    {
        std::swap(a, b);
    }

    if (b > c)
    {
        std::swap(b, c);
    }

    if (a > b)
    {
        std::swap(a, b);
    }

    return FaceKey{{a, b, c}};
}

bool containsAll(
    const std::unordered_set<std::size_t>& allowed,
    const std::array<std::size_t, 3>& faceNodes)
{
    return allowed.contains(faceNodes[0])
        && allowed.contains(faceNodes[1])
        && allowed.contains(faceNodes[2]);
}

} // namespace

void BoundaryFaceCollector::collect(
    MeshGroups& groups,
    const Mesh& mesh)
{
    groups.face_sets.clear();

    struct RegionMembership
    {
        std::string name;
        std::unordered_set<std::size_t> node_ids;
    };

    std::vector<RegionMembership> regions;
    regions.reserve(groups.node_sets.size());

    std::unordered_set<std::size_t> allBoundaryNodes;

    for (const auto& [regionName, nodeIds] : groups.node_sets)
    {
        groups.face_sets.emplace(
            regionName,
            std::vector<BoundaryFace>{});

        RegionMembership region;
        region.name = regionName;
        region.node_ids.reserve(nodeIds.size());

        for (const std::size_t nodeId : nodeIds)
        {
            region.node_ids.insert(nodeId);
            allBoundaryNodes.insert(nodeId);
        }

        regions.push_back(std::move(region));
    }

    const auto& elements = mesh.elements();

    std::unordered_map<
        FaceKey,
        FaceRecord,
        FaceKeyHash
    > faceMap;

    // Reserving 4 * elements is safe but often excessive because most faces
    // are internal. This is a reasonable initial estimate.
    faceMap.reserve(elements.size());

    auto addFace =
        [&faceMap, &allBoundaryNodes](
            std::size_t a,
            std::size_t b,
            std::size_t c)
    {
        /*
         * A true external face must consist only of nodes known to belong
         * to the model boundary. This skips the great majority of internal
         * tetrahedral faces before they reach the hash map.
         */
        if (!allBoundaryNodes.contains(a)
            || !allBoundaryNodes.contains(b)
            || !allBoundaryNodes.contains(c))
        {
            return;
        }

        const FaceKey key = makeFaceKey(a, b, c);

        const auto [iterator, inserted] =
            faceMap.try_emplace(
                key,
                FaceRecord{{a, b, c}, 1});

        if (!inserted)
        {
            ++iterator->second.count;
        }
    };

    for (const Element& element : elements)
    {
        if (element.type != ElementType::Tet4)
        {
            throw std::invalid_argument(
                "BoundaryFaceCollector currently supports Tet4 only.");
        }

        if (element.node_ids.size() != 4)
        {
            throw std::invalid_argument(
                "Tet4 element must have exactly four node IDs.");
        }

        const auto& n = element.node_ids;

        addFace(n[0], n[1], n[2]);
        addFace(n[0], n[1], n[3]);
        addFace(n[0], n[2], n[3]);
        addFace(n[1], n[2], n[3]);
    }

    for (const auto& [key, record] : faceMap)
    {
        if (record.count != 1)
        {
            continue;
        }

        const RegionMembership* matchedRegion = nullptr;

        for (const RegionMembership& region : regions)
        {
            if (!containsAll(region.node_ids, record.node_ids))
            {
                continue;
            }

            if (matchedRegion != nullptr)
            {
                throw std::runtime_error(
                    "BoundaryFaceCollector: boundary face matched "
                    "multiple regions.");
            }

            matchedRegion = &region;
        }

        if (matchedRegion == nullptr)
        {
            throw std::runtime_error(
                "BoundaryFaceCollector: boundary face did not match "
                "a region.");
        }

        groups.face_sets[matchedRegion->name].push_back(
            BoundaryFace{{
                record.node_ids[0],
                record.node_ids[1],
                record.node_ids[2]
            }});
    }
}

} // namespace feast
