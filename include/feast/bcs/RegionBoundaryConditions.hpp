#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace feast{


    struct RegionDirichletCondition{

        std::string regionName;
        std::size_t dof{0};
        double value{0.0};

    };
    struct RegionNeumannCondition{

        std::string regionName;
        std::size_t dof{0};
        double value{0.0};

    };
    class RegionBoundaryConditionSet{
    public:
        RegionBoundaryConditionSet() = default;

        void addRegionDirichlet(const std::string& regionName, std::size_t dof, double value);
        void addRegionNeumann(const std::string& regionName, std::size_t dof, double value);

        const std::vector<RegionDirichletCondition>& regionDirichletConditions() const;
        const std::vector<RegionNeumannCondition>& regionNeumannConditions() const;

        std::size_t numRegionDirichlet() const;
        std::size_t numRegionNeumann() const;

        void clear();

    private:
        std::vector<RegionDirichletCondition> m_region_dirichlet;
        std::vector<RegionNeumannCondition> m_region_neumann;
    };

}
