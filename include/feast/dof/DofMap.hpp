#pragma once

#include <vector>
#include <cstddef>

namespace feast {

class DofMap {
public:
    DofMap() = default;

    void resize(std::size_t numNodes, std::size_t dofsPerNode);

    int dof(std::size_t nodeId, std::size_t localDof) const;

    std::vector<int> elementDofs(const std::vector<std::size_t>& nodeIds) const;

    std::size_t numNodes() const;

    std::size_t dofsPerNode() const;

    std::size_t numDofs() const;

private:
    std::size_t m_numNodes{0};
    std::size_t m_dofsPerNode{0};

    std::vector<int> m_dofs;
};

} // namespace feast
