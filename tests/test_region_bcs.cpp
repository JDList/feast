#include <cassert>
#include <stdexcept>

#include "feast/bcs/RegionBoundaryConditions.hpp"

int main()
{
    feast::RegionBoundaryConditionSet bcs;

    bcs.addRegionDirichlet("lowx", 0, 0.0);
    bcs.addRegionNeumann("highz", 2, -100.0);

    assert(bcs.numRegionDirichlet() == 1);
    assert(bcs.numRegionNeumann() == 1);

    assert(bcs.regionDirichletConditions().size() == 1);
    assert(bcs.regionNeumannConditions().size() == 1);

    assert(bcs.regionDirichletConditions().front().regionName == "lowx");
    assert(bcs.regionNeumannConditions().front().regionName == "highz");

    bool threw = false;
    try {
        bcs.addRegionDirichlet("", 0, 0.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    threw = false;
    try {
        bcs.addRegionNeumann("", 0, 0.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    bcs.clear();
    assert(bcs.numRegionDirichlet() == 0);
    assert(bcs.numRegionNeumann() == 0);

    return 0;
}
