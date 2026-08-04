#include "feast/assembly/GlobalAssembler.hpp"
#include "feast/elements/Tet4.hpp"

#include <stdexcept>
#include <utility>

namespace feast {

GlobalAssembler::GlobalAssembler(std::size_t numDofs)
    : m_numDofs(numDofs)
{
}

AssemblyResult GlobalAssembler::assembleLinearSystem(
    const Mesh& mesh,
    const DofMap& dofMap,
    const BoundaryConditionSet& boundaryConditions,
    const std::vector<DenseMatrix>& elementStiffnesses,
    const std::vector<Vector>& elementVectors) const
{
    const auto& elements = mesh.elements();
    const std::size_t numberOfElements = elements.size();

    if (elementStiffnesses.size() != numberOfElements)
    {
        throw std::invalid_argument(
            "Number of element stiffness matrices does not "
            "match number of mesh elements.");
    }

    if (!elementVectors.empty() &&
        elementVectors.size() != numberOfElements)
    {
        throw std::invalid_argument(
            "Number of element vectors does not match "
            "number of mesh elements.");
    }

    /*
     * Reserve the maximum possible number of entries.
     *
     * This remains generic:
     * - Tet4 stiffness: 12 x 12
     * - Hex8 stiffness: 24 x 24
     * - Beam/shell elements may have different sizes
     */
    std::size_t expectedTriplets = 0;

    for (const auto& elementStiffness : elementStiffnesses)
    {
        expectedTriplets +=
            elementStiffness.rows() *
            elementStiffness.cols();
    }

    std::vector<Triplet> triplets;
    triplets.reserve(expectedTriplets);

    Vector force;
    force.resize(m_numDofs);
    force.setZero();

    for (std::size_t elementIndex = 0;
         elementIndex < numberOfElements;
         ++elementIndex)
    {
        const auto& element = elements[elementIndex];

        const std::vector<int> dofIds =
            dofMap.elementDofs(element.node_ids);

        ElementAssembler::addStiffnessTriplets(
            dofIds,
            elementStiffnesses[elementIndex],
            triplets);

        if (!elementVectors.empty())
        {
            ElementAssembler::addVectorContribution(
                dofIds,
                elementVectors[elementIndex],
                force);
        }
    }

    for (const auto& neumann :
         boundaryConditions.neumannConditions())
    {
        const int globalDof =
            dofMap.dof(neumann.nodeId, neumann.dof);

        if (globalDof < 0)
        {
            throw std::runtime_error(
                "Invalid global DOF generated from "
                "Neumann condition.");
        }

        const std::size_t globalDofIndex =
            static_cast<std::size_t>(globalDof);

        if (globalDofIndex >= force.size())
        {
            throw std::out_of_range(
                "Neumann condition global DOF is out of range.");
        }

        force[globalDofIndex] += neumann.value;
    }

    SparseMatrix stiffness(m_numDofs, m_numDofs);
    stiffness.setFromTriplets(triplets);

    return AssemblyResult{
        std::move(stiffness),
        std::move(force)
    };
}
//overload that constructs Ke inside assembly loop for ram efficiency
AssemblyResult GlobalAssembler::assembleLinearSystem(
    const Mesh& mesh,
    const DofMap& dofMap,
    const BoundaryConditionSet& boundaryConditions,
    const std::vector<LinearElastic>& materials,
    const std::vector<Vector>& elementVectors) const
{
    const auto& elements = mesh.elements();

    if (!elementVectors.empty() && elementVectors.size() != elements.size()) {
        throw std::invalid_argument(
            "GlobalAssembler: number of element vectors does not match number of mesh elements.");
    }

    // Tet4: 12 x 12 = 144 possible entries per element.
    std::vector<Triplet> triplets;
    triplets.reserve(elements.size() * 144);

    Vector force;
    force.resize(m_numDofs);
    force.setZero();

    std::vector<int> dofIds;
    dofIds.reserve(12);

    for (std::size_t e = 0; e < elements.size(); ++e) {
        const Element& element = elements[e];

        if (element.material_id >= materials.size()) {
            throw std::out_of_range(
                "GlobalAssembler: element material_id is outside the materials table.");
        }

        const LinearElastic& material = materials[element.material_id];

        dofMap.elementDofs(element.node_ids, dofIds);
      
        const DenseMatrix ke =
            Tet4::stiffnessMatrix(mesh, element, material);

        ElementAssembler::addStiffnessTriplets(dofIds, ke, triplets);

        if (!elementVectors.empty()) {
            ElementAssembler::addVectorContribution(
                dofIds,
                elementVectors[e],
                force);
        }
    }

    for (const auto& neumann : boundaryConditions.neumannConditions()) {
        const int globalDof = dofMap.dof(neumann.nodeId, neumann.dof);
        if (globalDof < 0) {
            throw std::runtime_error(
                "GlobalAssembler: invalid global DOF generated from Neumann condition.");
        }

        const std::size_t index = static_cast<std::size_t>(globalDof);
        if (index >= force.size()) {
            throw std::out_of_range(
                "GlobalAssembler: Neumann condition global DOF is out of range.");
        }

        force[index] += neumann.value;
    }

    SparseMatrix stiffness(m_numDofs, m_numDofs);
    stiffness.setFromTriplets(triplets);

    return AssemblyResult{
        std::move(stiffness),
        std::move(force)
    };
}

} // namespace feast
