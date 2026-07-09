#include "feast/dof/DofMap.hpp"
#include "feast/elements/Tet4.hpp"
#include "feast/kernel/Kernel.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/postprocessing/PostProcessor.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

using namespace feast;

namespace {

bool approx(double a, double b, double tol = 1e-10)
{
    return std::abs(a - b) <= tol * (1.0 + std::max(std::abs(a), std::abs(b)));
}

Mesh makeUnitTetMesh()
{
    Mesh mesh;

    mesh.addNode(Node{0, 0.0, 0.0, 0.0});
    mesh.addNode(Node{1, 1.0, 0.0, 0.0});
    mesh.addNode(Node{2, 0.0, 1.0, 0.0});
    mesh.addNode(Node{3, 0.0, 0.0, 1.0});

    Element element;
    element.id = 0;
    element.type = ElementType::Tet4;
    element.node_ids = {0, 1, 2, 3};
    element.material_id = 0;

    mesh.addElement(element);

    return mesh;
}

LinearElastic makeMaterial()
{
    return LinearElastic(210e9, 0.30);
}

LinearStaticResult makeResult()
{
    LinearStaticResult result;
    result.solution.resize(12);

    // Affine displacement field:
    // u_x = 0.01 * x
    // u_y = 0.02 * y
    // u_z = 0.03 * z

    result.solution[0]  = 0.0;
    result.solution[1]  = 0.0;
    result.solution[2]  = 0.0;

    result.solution[3]  = 0.01;
    result.solution[4]  = 0.0;
    result.solution[5]  = 0.0;

    result.solution[6]  = 0.0;
    result.solution[7]  = 0.02;
    result.solution[8]  = 0.0;

    result.solution[9]  = 0.0;
    result.solution[10] = 0.0;
    result.solution[11] = 0.03;

    return result;
}

DofMap makeIdentityDofMap()
{
    DofMap dofMap;
    dofMap.resize(4, 3);
    return dofMap;
}

void testTet4Recovery()
{
    const Mesh mesh = makeUnitTetMesh();
    const Element& element = mesh.elements().at(0);
    const LinearElastic material = makeMaterial();

    Vector ue;
    ue.resize(12);

    ue[0]  = 0.0;
    ue[1]  = 0.0;
    ue[2]  = 0.0;
    ue[3]  = 0.01;
    ue[4]  = 0.0;
    ue[5]  = 0.0;
    ue[6]  = 0.0;
    ue[7]  = 0.02;
    ue[8]  = 0.0;
    ue[9]  = 0.0;
    ue[10] = 0.0;
    ue[11] = 0.03;

    const Vector eps = Tet4::strain(mesh, element, ue);
    const Vector sig = Tet4::stress(mesh, element, material, ue);

    assert(eps.size() == 6);
    assert(sig.size() == 6);

    assert(approx(eps[0], 0.01));
    assert(approx(eps[1], 0.02));
    assert(approx(eps[2], 0.03));
    assert(approx(eps[3], 0.0));
    assert(approx(eps[4], 0.0));
    assert(approx(eps[5], 0.0));

    const double E = material.youngsModulus();
    const double nu = material.poissonRatio();
    const double lambda = (E * nu) / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double mu = E / (2.0 * (1.0 + nu));
    const double trace = 0.01 + 0.02 + 0.03;

    const double sxx = lambda * trace + 2.0 * mu * 0.01;
    const double syy = lambda * trace + 2.0 * mu * 0.02;
    const double szz = lambda * trace + 2.0 * mu * 0.03;

    assert(approx(sig[0], sxx));
    assert(approx(sig[1], syy));
    assert(approx(sig[2], szz));
    assert(approx(sig[3], 0.0));
    assert(approx(sig[4], 0.0));
    assert(approx(sig[5], 0.0));
}

void testPostProcessor()
{
    const Mesh mesh = makeUnitTetMesh();
    const LinearElastic material = makeMaterial();
    const LinearStaticResult result = makeResult();
    const DofMap dofMap = makeIdentityDofMap();
    const std::vector<LinearElastic> materials{material};

    const PostProcessResult pp =
        PostProcessor::process(result, mesh, dofMap, materials);

    assert(pp.displacement.values.size() == 4);

    assert(approx(pp.displacement.values[0][0], 0.0));
    assert(approx(pp.displacement.values[0][1], 0.0));
    assert(approx(pp.displacement.values[0][2], 0.0));

    assert(approx(pp.displacement.values[1][0], 0.01));
    assert(approx(pp.displacement.values[1][1], 0.0));
    assert(approx(pp.displacement.values[1][2], 0.0));

    assert(approx(pp.displacement.values[2][0], 0.0));
    assert(approx(pp.displacement.values[2][1], 0.02));
    assert(approx(pp.displacement.values[2][2], 0.0));

    assert(approx(pp.displacement.values[3][0], 0.0));
    assert(approx(pp.displacement.values[3][1], 0.0));
    assert(approx(pp.displacement.values[3][2], 0.03));

    assert(pp.displacement.magnitude.summary.count == 4);
    assert(approx(pp.displacement.magnitude.summary.max, 0.03));
    assert(approx(pp.displacement.magnitude.summary.min, 0.0));

    assert(pp.strain.values.size() == 1);
    assert(approx(pp.strain.values[0][0], 0.01));
    assert(approx(pp.strain.values[0][1], 0.02));
    assert(approx(pp.strain.values[0][2], 0.03));
    assert(approx(pp.strain.values[0][3], 0.0));
    assert(approx(pp.strain.values[0][4], 0.0));
    assert(approx(pp.strain.values[0][5], 0.0));

    assert(pp.stress.values.size() == 1);
    assert(pp.stress.vonMises.values.size() == 1);
    assert(pp.stress.hydrostatic.values.size() == 1);
    assert(pp.stress.principal.values.size() == 1);

    const double E = material.youngsModulus();
    const double nu = material.poissonRatio();
    const double lambda = (E * nu) / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double mu = E / (2.0 * (1.0 + nu));
    const double trace = 0.01 + 0.02 + 0.03;

    const double sxx = lambda * trace + 2.0 * mu * 0.01;
    const double syy = lambda * trace + 2.0 * mu * 0.02;
    const double szz = lambda * trace + 2.0 * mu * 0.03;

    assert(approx(pp.stress.values[0][0], sxx));
    assert(approx(pp.stress.values[0][1], syy));
    assert(approx(pp.stress.values[0][2], szz));
    assert(approx(pp.stress.values[0][3], 0.0));
    assert(approx(pp.stress.values[0][4], 0.0));
    assert(approx(pp.stress.values[0][5], 0.0));

    const double hydro = (sxx + syy + szz) / 3.0;
    assert(approx(pp.stress.hydrostatic.values[0], hydro));

    const double pmin = std::min({sxx, syy, szz});
    const double pmax = std::max({sxx, syy, szz});
    const double pmid = (sxx + syy + szz) - pmin - pmax;

    assert(approx(pp.stress.principal.values[0][0], pmin));
    assert(approx(pp.stress.principal.values[0][1], pmid));
    assert(approx(pp.stress.principal.values[0][2], pmax));

    const double vm = std::sqrt(
        0.5 * ((sxx - syy) * (sxx - syy) +
               (syy - szz) * (syy - szz) +
               (szz - sxx) * (szz - sxx)));

    assert(approx(pp.stress.vonMises.values[0], vm));
    assert(approx(pp.stress.vonMises.summary.max, vm));
    assert(approx(pp.stress.vonMises.summary.min, vm));
    assert(approx(pp.stress.vonMises.summary.mean, vm));
}

} // namespace

int main()
{
    testTet4Recovery();
    testPostProcessor();

    std::cout << "test_postprocessing passed\n";
    return 0;
}
