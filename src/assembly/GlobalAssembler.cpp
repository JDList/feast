#include "feast/assembly/GlobalAssembler.hpp"

#include <stdexcept>

namespace feast {

GlobalAssembler::GlobalAssembler(std::size_t numDofs)
    : m_numDofs(numDofs)
{
}

AssemblyResult GlobalAssembler::assembleLinearSystem(const Mesh& mesh,
                                                     const DofMap& dofMap,
                                                     const BoundaryConditionSet& boundaryConditions,
                                                     const std::vector<DenseMatrix>& elementStiffnesses,
                                                     const std::vector<Vector>& elementVectors) const
{
    if (elementStiffnesses.size() != mesh.elements().size())
        throw std::invalid_argument("Number of element stiffness matrices does not match number of mesh elements.");

    if (!elementVectors.empty() && elementVectors.size() != mesh.elements().size())
        throw std::invalid_argument("Number of element vectors does not match number of mesh elements.");

    std::vector<Triplet> triplets;
    Vector force;
    force.resize(m_numDofs);
    force.setZero();

    for (std::size_t e = 0; e < mesh.elements().size(); ++e)
    {
        const auto& element = mesh.elements()[e];
        const std::vector<int> dofIds = dofMap.elementDofs(element.node_ids);

        ElementAssembler::addStiffnessTriplets(dofIds, elementStiffnesses[e], triplets);

        if (!elementVectors.empty())
            ElementAssembler::addVectorContribution(dofIds, elementVectors[e], force);
    }

    for (const auto& neumann : boundaryConditions.neumannConditions())
    {
        const int globalDof = dofMap.dof(neumann.nodeId, neumann.dof);
        if (globalDof < 0)
            throw std::runtime_error("Invalid global DOF generated from Neumann condition.");

        force[static_cast<std::size_t>(globalDof)] += neumann.value;
    }

    SparseMatrix K;
    K.resize(static_cast<int>(m_numDofs), static_cast<int>(m_numDofs));
    K.setZero();
    K.setFromTriplets(triplets);

    AssemblyResult result;
    result.stiffness = K;
    result.force = force;
    return result;
}

} // namespace feast
