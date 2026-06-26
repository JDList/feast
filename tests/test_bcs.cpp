#include <cassert>

#include "feast/bcs/BoundaryConditions.hpp"

int main()
{
    feast::BoundaryConditionSet bcs;

    assert(bcs.numDirichlet() == 0);
    assert(bcs.numNeumann() == 0);

    bcs.addDirichlet(0, 0, 0.0);
    bcs.addDirichlet(3, 2, 1.5);
    bcs.addNeumann(1, 1, -25.0);

    assert(bcs.numDirichlet() == 2);
    assert(bcs.numNeumann() == 1);

    const auto& dirichlet = bcs.dirichletConditions();
    const auto& neumann = bcs.neumannConditions();

    assert(dirichlet[0].nodeId == 0);
    assert(dirichlet[0].dof == 0);
    assert(dirichlet[0].value == 0.0);

    assert(dirichlet[1].nodeId == 3);
    assert(dirichlet[1].dof == 2);
    assert(dirichlet[1].value == 1.5);

    assert(neumann[0].nodeId == 1);
    assert(neumann[0].dof == 1);
    assert(neumann[0].value == -25.0);

    bcs.clear();
    assert(bcs.numDirichlet() == 0);
    assert(bcs.numNeumann() == 0);

    return 0;
}
