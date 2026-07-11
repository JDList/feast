#include <cstddef>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/meshing/BoundaryFaceCollector.hpp"
#include "feast/meshing/MeshBuildResult.hpp"
#include "feast/meshing/StructuredTetMesher.hpp"

namespace py = pybind11;

void bindMeshing(py::module_& m)
{
    // MeshGroups is part of the meshing layer and should be bound from the
    // same header that defines MeshBuildResult, not from a separate MeshGroups.hpp.
    py::class_<feast::BoundaryFace>(m, "BoundaryFace")
        .def(py::init<>())
        .def_readwrite("node_ids", &feast::BoundaryFace::node_ids);

    py::class_<feast::MeshGroups>(m, "MeshGroups")
        .def(py::init<>())
        .def_readwrite("node_sets", &feast::MeshGroups::node_sets)
        .def_readwrite("face_sets", &feast::MeshGroups::face_sets)
        .def("clear", &feast::MeshGroups::clear);

    py::class_<feast::MeshBuildResult>(m, "MeshBuildResult")
        .def(py::init<>())
        .def_readwrite("mesh", &feast::MeshBuildResult::mesh)
        .def_readwrite("groups", &feast::MeshBuildResult::groups);

    py::class_<feast::BoundaryFaceCollector>(m, "BoundaryFaceCollector")
        .def_static("collect", &feast::BoundaryFaceCollector::collect,
                    py::arg("groups"), py::arg("mesh"));

    py::class_<feast::StructuredTetMesher>(m, "StructuredTetMesher")
        .def(py::init<double>(), py::arg("target_element_size"))
        .def("generate", &feast::StructuredTetMesher::generate, py::arg("geometry"));
}
