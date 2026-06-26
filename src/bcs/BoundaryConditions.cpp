#include "feast/bcs/BoundaryConditions.hpp"

namespace feast {

void BoundaryConditionSet::addDirichlet(std::size_t nodeId, std::size_t dof, double value)
{
    m_dirichlet.push_back(DirichletCondition{nodeId, dof, value});
}

void BoundaryConditionSet::addNeumann(std::size_t nodeId, std::size_t dof, double value)
{
    m_neumann.push_back(NeumannCondition{nodeId, dof, value});
}

const std::vector<DirichletCondition>& BoundaryConditionSet::dirichletConditions() const
{
    return m_dirichlet;
}

const std::vector<NeumannCondition>& BoundaryConditionSet::neumannConditions() const
{
    return m_neumann;
}

std::size_t BoundaryConditionSet::numDirichlet() const
{
    return m_dirichlet.size();
}

std::size_t BoundaryConditionSet::numNeumann() const
{
    return m_neumann.size();
}

void BoundaryConditionSet::clear()
{
    m_dirichlet.clear();
    m_neumann.clear();
}

} // namespace feast
