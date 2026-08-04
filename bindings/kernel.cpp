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
        .def_readwrite("stiffness", &feast::LinearStaticResult::stiffness)
        .def_readwrite("force", &feast::LinearStaticResult::force)
        .def_readwrite("solution", &feast::LinearStaticResult::solution);

    py::class_<feast::Kernel>(m, "Kernel")
        .def(py::init<const feast::LinearSolver&>(),
             py::arg("solver"),
             py::keep_alive<1, 2>())
        .def("solveLinearStatic",
             py::overload_cast<
                 const feast::Mesh&,
                 const feast::DofMap&,
                 const feast::BoundaryConditionSet&,
                 const std::vector<feast::DenseMatrix>&,
                 const std::vector<feast::Vector>&
             >(&feast::Kernel::solveLinearStatic, py::const_),
             py::arg("mesh"),
             py::arg("dofMap"),
             py::arg("boundaryConditions"),
             py::arg("elementStiffnesses"),
             py::arg("elementVectors") = std::vector<feast::Vector>{})
        .def("solveLinearStatic",
             py::overload_cast<
                 const feast::Mesh&,
                 const feast::DofMap&,
                 const feast::BoundaryConditionSet&,
                 const std::vector<feast::LinearElastic>&,
                 const std::vector<feast::Vector>&
             >(&feast::Kernel::solveLinearStatic, py::const_),
             py::arg("mesh"),
             py::arg("dofMap"),
             py::arg("boundaryConditions"),
             py::arg("materials"),
             py::arg("elementVectors") = std::vector<feast::Vector>{});
}
