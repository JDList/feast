#include "feast/bcs/RegionBoundaryConditions.hpp"

#include <stdexcept>
#include <vector>

namespace feast {

void RegionBoundaryConditionSet::addRegionDirichlet(
    const std::string& regionName,
    std::size_t dof,
    double value)
{
    if (regionName.empty()) {
        throw std::invalid_argument("Region Dirichlet condition requires a non-empty region name.");
    }

    m_region_dirichlet.push_back(RegionDirichletCondition{regionName, dof, value});
}

void RegionBoundaryConditionSet::addRegionNeumann(
    const std::string& regionName,
    std::size_t dof,
    double value)
{
    if (regionName.empty()) {
        throw std::invalid_argument("Region Neumann condition requires a non-empty region name.");
    }

    m_region_neumann.push_back(RegionNeumannCondition{regionName, dof, value});
}

const std::vector<RegionDirichletCondition>&
RegionBoundaryConditionSet::regionDirichletConditions() const
{
    return m_region_dirichlet;
}

const std::vector<RegionNeumannCondition>&
RegionBoundaryConditionSet::regionNeumannConditions() const
{
    return m_region_neumann;
}

std::size_t RegionBoundaryConditionSet::numRegionDirichlet() const
{
    return m_region_dirichlet.size();
}

std::size_t RegionBoundaryConditionSet::numRegionNeumann() const
{
    return m_region_neumann.size();
}

void RegionBoundaryConditionSet::clear()
{
    m_region_dirichlet.clear();
    m_region_neumann.clear();
}

} // namespace feast
