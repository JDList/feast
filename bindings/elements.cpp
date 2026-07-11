#include <pybind11/pybind11.h>

#include "feast/elements/Tet4.hpp"

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
}
