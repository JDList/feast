#include <pybind11/pybind11.h>

#include "feast/dof/DofMap.hpp"

namespace py = pybind11;

void bindDof(py::module_& m)
{
    py::class_<feast::DofMap>(m, "DofMap")
        .def(py::init<>())
        .def("resize", &feast::DofMap::resize,
             py::arg("numNodes"), py::arg("dofsPerNode"))
        .def("dof", &feast::DofMap::dof,
             py::arg("nodeId"), py::arg("localDof"));
}
