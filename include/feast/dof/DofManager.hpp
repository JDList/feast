#pragma once

#include "feast/dof/DofMap.hpp"
#include "feast/mesh/Mesh.hpp"

namespace feast {

class DofManager {
public:
    explicit DofManager(std::size_t dofsPerNode = 3);

    void build(const Mesh& mesh);

    const DofMap& dofMap() const;

    std::size_t numDofs() const;

private:
    std::size_t m_dofsPerNode;
    DofMap m_dofMap;
};

} // namespace feast
