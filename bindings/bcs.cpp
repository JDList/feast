#include <cstddef>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/bcs/BoundaryConditionResolver.hpp"
#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/bcs/RegionBoundaryConditions.hpp"

namespace py = pybind11;

void bindBoundaryConditions(py::module_& m)
{
    py::class_<feast::DirichletCondition>(m, "DirichletCondition")
        .def(py::init<>())
        .def_readwrite("nodeId", &feast::DirichletCondition::nodeId)
        .def_readwrite("dof", &feast::DirichletCondition::dof)
        .def_readwrite("value", &feast::DirichletCondition::value);

    py::class_<feast::NeumannCondition>(m, "NeumannCondition")
        .def(py::init<>())
        .def_readwrite("nodeId", &feast::NeumannCondition::nodeId)
        .def_readwrite("dof", &feast::NeumannCondition::dof)
        .def_readwrite("value", &feast::NeumannCondition::value);

    py::class_<feast::BoundaryConditionSet>(m, "BoundaryConditionSet")
        .def(py::init<>())
        .def("addDirichlet", &feast::BoundaryConditionSet::addDirichlet,
             py::arg("nodeId"), py::arg("dof"), py::arg("value"))
        .def("addNeumann", &feast::BoundaryConditionSet::addNeumann,
             py::arg("nodeId"), py::arg("dof"), py::arg("value"))
        .def("dirichletConditions", &feast::BoundaryConditionSet::dirichletConditions,
             py::return_value_policy::reference_internal)
        .def("neumannConditions", &feast::BoundaryConditionSet::neumannConditions,
             py::return_value_policy::reference_internal)
        .def("numDirichlet", &feast::BoundaryConditionSet::numDirichlet)
        .def("numNeumann", &feast::BoundaryConditionSet::numNeumann)
        .def("clear", &feast::BoundaryConditionSet::clear);

    py::class_<feast::RegionDirichletCondition>(m, "RegionDirichletCondition")
        .def(py::init<>())
        .def_readwrite("regionName", &feast::RegionDirichletCondition::regionName)
        .def_readwrite("dof", &feast::RegionDirichletCondition::dof)
        .def_readwrite("value", &feast::RegionDirichletCondition::value);

    py::class_<feast::RegionNeumannCondition>(m, "RegionNeumannCondition")
        .def(py::init<>())
        .def_readwrite("regionName", &feast::RegionNeumannCondition::regionName)
        .def_readwrite("dof", &feast::RegionNeumannCondition::dof)
        .def_readwrite("value", &feast::RegionNeumannCondition::value);

    py::class_<feast::RegionBoundaryConditionSet>(m, "RegionBoundaryConditionSet")
        .def(py::init<>())
        .def("addRegionDirichlet", &feast::RegionBoundaryConditionSet::addRegionDirichlet,
             py::arg("regionName"), py::arg("dof"), py::arg("value"))
        .def("addRegionNeumann", &feast::RegionBoundaryConditionSet::addRegionNeumann,
             py::arg("regionName"), py::arg("dof"), py::arg("value"));

    py::class_<feast::BoundaryConditionResolver>(m, "BoundaryConditionResolver")
        .def_static("resolve", &feast::BoundaryConditionResolver::resolve,
                    py::arg("regionConditions"), py::arg("buildResult"))
        .def_static("resolveDirichlet", &feast::BoundaryConditionResolver::resolveDirichlet,
                    py::arg("regionConditions"), py::arg("buildResult"))
        .def_static("resolveNeumann", &feast::BoundaryConditionResolver::resolveNeumann,
                    py::arg("regionConditions"), py::arg("buildResult"));
}
