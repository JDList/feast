#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/kernel/Kernel.hpp"
#include "feast/solvers/LinearSolver.hpp"

namespace py = pybind11;

void bindKernel(py::module_& m)
{
    py::class_<feast::LinearStaticResult>(m, "LinearStaticResult")
        .def(py::init<>())
        .def_readwrite("solution", &feast::LinearStaticResult::solution);

    py::class_<feast::Kernel>(m, "Kernel")
        .def(py::init<const feast::LinearSolver&>(),
             py::arg("solver"),
             py::keep_alive<1, 2>())
        .def("solveLinearStatic", &feast::Kernel::solveLinearStatic,
             py::arg("mesh"),
             py::arg("dofMap"),
             py::arg("boundaryConditions"),
             py::arg("elementStiffnesses"),
             py::arg("elementVectors") = std::vector<feast::Vector>{});
}
