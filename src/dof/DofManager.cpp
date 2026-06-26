#include "feast/dof/DofManager.hpp"

namespace feast {

DofManager::DofManager(std::size_t dofsPerNode)
    : m_dofsPerNode(dofsPerNode)
{
}

void DofManager::build(const Mesh& mesh)
{
    m_dofMap.resize(mesh.nodes().size(), m_dofsPerNode);
}

const DofMap& DofManager::dofMap() const
{
    return m_dofMap;
}

std::size_t DofManager::numDofs() const
{
    return m_dofMap.numDofs();
}

} // namespace feast
