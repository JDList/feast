#include <cstddef>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/mesh/Mesh.hpp"

namespace py = pybind11;

void bindMesh(py::module_& m)
{
    py::class_<feast::Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("id", &feast::Node::id)
        .def_readwrite("x", &feast::Node::x)
        .def_readwrite("y", &feast::Node::y)
        .def_readwrite("z", &feast::Node::z);

    py::class_<feast::Element>(m, "Element")
        .def(py::init<>())
        .def_readwrite("id", &feast::Element::id)
        .def_readwrite("type", &feast::Element::type)
        .def_readwrite("node_ids", &feast::Element::node_ids)
        .def_readwrite("material_id", &feast::Element::material_id);

    py::class_<feast::Mesh>(m, "Mesh")
        .def(py::init<>())
        .def("addNode", &feast::Mesh::addNode, py::arg("node"))
        .def("addElement", &feast::Mesh::addElement, py::arg("element"))
        .def("nodes", &feast::Mesh::nodes, py::return_value_policy::reference_internal)
        .def("elements", &feast::Mesh::elements, py::return_value_policy::reference_internal)
        .def("validate", &feast::Mesh::validate);
}
