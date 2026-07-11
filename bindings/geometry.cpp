#include <cstddef>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/geometry/FaceTag.hpp"
#include "feast/geometry/Geometry.hpp"
#include "feast/geometry/builders/CuboidBuilder.hpp"
#include "feast/geometry/primitives/Cuboid.hpp"

namespace py = pybind11;

void bindGeometry(py::module_& m)
{
    py::class_<feast::Point3D>(m, "Point3D")
        .def(py::init<>())
        .def(py::init<double, double, double>(),
             py::arg("x"), py::arg("y"), py::arg("z"))
        .def_readwrite("x", &feast::Point3D::x)
        .def_readwrite("y", &feast::Point3D::y)
        .def_readwrite("z", &feast::Point3D::z);

    py::enum_<feast::FaceTag>(m, "FaceTag")
        .value("LowX", feast::FaceTag::LowX)
        .value("HighX", feast::FaceTag::HighX)
        .value("LowY", feast::FaceTag::LowY)
        .value("HighY", feast::FaceTag::HighY)
        .value("LowZ", feast::FaceTag::LowZ)
        .value("HighZ", feast::FaceTag::HighZ)
        .export_values();

    py::class_<feast::Surface>(m, "Surface")
        .def(py::init<>())
        .def(py::init<std::string, feast::FaceTag, std::vector<std::size_t>>(),
             py::arg("name"), py::arg("tag"), py::arg("vertex_ids"))
        .def_readwrite("name", &feast::Surface::name)
        .def_readwrite("tag", &feast::Surface::tag)
        .def_readwrite("vertex_ids", &feast::Surface::vertex_ids);

    py::class_<feast::Geometry>(m, "Geometry")
        .def(py::init<>())
        .def("addVertex", &feast::Geometry::addVertex, py::arg("point"))
        .def("addSurface", &feast::Geometry::addSurface, py::arg("surface"))
        .def("vertices", &feast::Geometry::vertices, py::return_value_policy::reference_internal)
        .def("surfaces", &feast::Geometry::surfaces, py::return_value_policy::reference_internal)
        .def("clear", &feast::Geometry::clear)
        .def("validate", &feast::Geometry::validate);

    py::class_<feast::Cuboid>(m, "Cuboid")
        .def(py::init<double, double, double>(),
             py::arg("lx"), py::arg("ly"), py::arg("lz"))
        .def_property_readonly("lengthX", &feast::Cuboid::lengthX)
        .def_property_readonly("lengthY", &feast::Cuboid::lengthY)
        .def_property_readonly("lengthZ", &feast::Cuboid::lengthZ);

    py::class_<feast::CuboidBuilder>(m, "CuboidBuilder")
        .def(py::init<>())
        .def("build", &feast::CuboidBuilder::build, py::arg("cuboid"));
}
