#include <cassert>
#include <cstddef>
#include <stdexcept>

#include "feast/mesh/Element.hpp"
#include "feast/mesh/ElementType.hpp"
#include "feast/mesh/Mesh.hpp"
#include "feast/mesh/Node.hpp"

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

    mesh.addElement(e);

    assert(mesh.nodes().size() == 4);
    assert(mesh.elements().size() == 1);

    bool threw = false;
    try {
        mesh.validate();
    } catch (const std::exception&) {
        threw = true;
    }

    assert(!threw);

    feast::Mesh bad_mesh;
    bad_mesh.addNode(feast::Node{0, 0.0, 0.0, 0.0});
    bad_mesh.addNode(feast::Node{1, 1.0, 0.0, 0.0});
    bad_mesh.addNode(feast::Node{2, 0.0, 1.0, 0.0});
    bad_mesh.addNode(feast::Node{3, 0.0, 0.0, 1.0});

    feast::Element bad_e;
    bad_e.id = 1;
    bad_e.type = feast::ElementType::Tet4;
    bad_e.material_id = 0;
    bad_e.node_ids = {0, 1, 2, 9};

    bad_mesh.addElement(bad_e);

    threw = false;
    try {
        bad_mesh.validate();
    } catch (const std::exception&) {
        threw = true;
    }

    assert(threw);

    return 0;
}
