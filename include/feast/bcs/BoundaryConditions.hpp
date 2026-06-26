#pragma once

#include <cstddef>
#include <vector>

namespace feast {

struct DirichletCondition
{
    std::size_t nodeId{0};
    std::size_t dof{0};     // local DOF index on the node
    double value{0.0};
};

struct NeumannCondition
{
    std::size_t nodeId{0};
    std::size_t dof{0};     // local DOF index on the node
    double value{0.0};
};

class BoundaryConditionSet
{
public:
    BoundaryConditionSet() = default;

    void addDirichlet(std::size_t nodeId, std::size_t dof, double value);
    void addNeumann(std::size_t nodeId, std::size_t dof, double value);

    const std::vector<DirichletCondition>& dirichletConditions() const;
    const std::vector<NeumannCondition>& neumannConditions() const;

    std::size_t numDirichlet() const;
    std::size_t numNeumann() const;

    void clear();

private:
    std::vector<DirichletCondition> m_dirichlet;
    std::vector<NeumannCondition> m_neumann;
};

} // namespace feast
