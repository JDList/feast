#include <cassert>
#include <cmath>
#include <stdexcept>

#include "feast/elements/Tet4.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/mesh/Element.hpp"
#include "feast/mesh/ElementType.hpp"
#include "feast/mesh/Mesh.hpp"
#include "feast/mesh/Node.hpp"

namespace {
bool almostEqual(double a, double b, double eps = 1e-10)
{
    return std::abs(a - b) < eps;
}
}

int main()
{
    feast::Mesh mesh;
    mesh.addNode(feast::Node{0, 0.0, 0.0, 0.0});
    mesh.addNode(feast::Node{1, 1.0, 0.0, 0.0});
    mesh.addNode(feast::Node{2, 0.0, 1.0, 0.0});
    mesh.addNode(feast::Node{3, 0.0, 0.0, 1.0});

    feast::Element e;
    e.id = 0;
    e.type = feast::ElementType::Tet4;
    e.material_id = 0;
    e.node_ids = {0, 1, 2, 3};

    feast::LinearElastic mat(210e9, 0.30, 7850.0);

    feast::DenseMatrix Ke = feast::Tet4::stiffnessMatrix(mesh, e, mat);

    assert(Ke.rows() == 12);
    assert(Ke.cols() == 12);

    for (std::size_t i = 0; i < Ke.rows(); ++i) {
        for (std::size_t j = 0; j < Ke.cols(); ++j) {
            assert(almostEqual(Ke(i, j), Ke(j, i)));
        }
    }

    bool threw = false;
    try {
        feast::Mesh bad_mesh;
        bad_mesh.addNode(feast::Node{0, 0.0, 0.0, 0.0});
        bad_mesh.addNode(feast::Node{1, 1.0, 0.0, 0.0});
        bad_mesh.addNode(feast::Node{2, 2.0, 0.0, 0.0});
        bad_mesh.addNode(feast::Node{3, 3.0, 0.0, 0.0});

        feast::Element bad_e = e;
        bad_e.node_ids = {0, 1, 2, 3};

        (void)feast::Tet4::stiffnessMatrix(bad_mesh, bad_e, mat);
    } catch (const std::exception&) {
        threw = true;
    }
    assert(threw);

    return 0;
}
