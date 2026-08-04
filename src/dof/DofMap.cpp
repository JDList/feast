#include "feast/dof/DofMap.hpp"

#include <stdexcept>

namespace feast {

void DofMap::resize(std::size_t numNodes, std::size_t dofsPerNode)
{
    m_numNodes = numNodes;
    m_dofsPerNode = dofsPerNode;

    m_dofs.resize(numNodes * dofsPerNode);

    int counter = 0;

    for (auto& dof : m_dofs)
        dof = counter++;
}

int DofMap::dof(std::size_t nodeId, std::size_t localDof) const
{
    if (nodeId >= m_numNodes)
        throw std::out_of_range("Invalid node ID.");

    if (localDof >= m_dofsPerNode)
        throw std::out_of_range("Invalid local DOF.");

    return m_dofs[nodeId * m_dofsPerNode + localDof];
}

void DofMap::elementDofs(
    const std::vector<std::size_t>& nodeIds,
    std::vector<int>& dofIds) const
{
    dofIds.clear();
    dofIds.reserve(nodeIds.size() * m_dofsPerNode);

    for (const std::size_t nodeId : nodeIds)
    {
        for (std::size_t localDof = 0; localDof < m_dofsPerNode; ++localDof)
        {
            dofIds.push_back(dof(nodeId, localDof));
        }
    }
}

std::vector<int> DofMap::elementDofs(const std::vector<std::size_t>& nodeIds) const
{
    std::vector<int> dofIds;
    elementDofs(nodeIds, dofIds);
    return dofIds;
}



std::size_t DofMap::numNodes() const
{
    return m_numNodes;
}

std::size_t DofMap::dofsPerNode() const
{
    return m_dofsPerNode;
}

std::size_t DofMap::numDofs() const
{
    return m_dofs.size();
}

} // namespace feast
