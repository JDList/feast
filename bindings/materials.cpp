#include <pybind11/pybind11.h>

#include "feast/materials/LinearElastic.hpp"

namespace py = pybind11;

void bindMaterials(py::module_& m)
{
    py::class_<feast::LinearElastic>(m, "LinearElastic")
        .def(py::init<double, double>(),
             py::arg("youngsModulus"), py::arg("poissonsRatio"));
}
