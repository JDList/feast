#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/elements/Tet4.hpp"
#include "feast/elements/ElementMatrixBuilder.hpp"

namespace py = pybind11;

void bindElements(py::module_& m)
{
    py::class_<feast::Tet4>(m, "Tet4")
        .def(py::init<>())
        .def_static("stiffnessMatrix", &feast::Tet4::stiffnessMatrix,
                    py::arg("mesh"), py::arg("element"), py::arg("material"))
        .def_static("strain", &feast::Tet4::strain,
                    py::arg("mesh"), py::arg("element"), py::arg("elementDisplacements"))
        .def_static("stress", &feast::Tet4::stress,
                    py::arg("mesh"), py::arg("element"), py::arg("material"), py::arg("elementDisplacements"));
    py::class_<feast::ElementMatrixBuilder>(
        m,
        "ElementMatrixBuilder")
        .def_static(
            "buildStiffnesses",
            py::overload_cast<
                const feast::Mesh&,
                const feast::LinearElastic&
            >(
                &feast::ElementMatrixBuilder::buildStiffnesses
            ),
            py::arg("mesh"),
            py::arg("material"),
            py::call_guard<py::gil_scoped_release>())
        .def_static(
            "buildStiffnesses",
            py::overload_cast<
                const feast::Mesh&,
                const std::vector<feast::LinearElastic>&
            >(
                &feast::ElementMatrixBuilder::buildStiffnesses
            ),
            py::arg("mesh"),
            py::arg("materials"),
            py::call_guard<py::gil_scoped_release>());
}
