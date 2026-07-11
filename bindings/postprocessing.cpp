#include <array>
#include <cstddef>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/dof/DofMap.hpp"
#include "feast/kernel/Kernel.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/mesh/Mesh.hpp"
#include "feast/postprocessing/PostProcessingTypes.hpp"
#include "feast/postprocessing/PostProcessor.hpp"

namespace py = pybind11;

void bindPostProcessing(py::module_& m)
{
    py::class_<feast::ScalarSummary>(m, "ScalarSummary")
        .def(py::init<>())
        .def_readwrite("count", &feast::ScalarSummary::count)
        .def_readwrite("sum", &feast::ScalarSummary::sum)
        .def_readwrite("mean", &feast::ScalarSummary::mean)
        .def_readwrite("min", &feast::ScalarSummary::min)
        .def_readwrite("max", &feast::ScalarSummary::max)
        .def_readwrite("range", &feast::ScalarSummary::range)
        .def_readwrite("standardDeviation", &feast::ScalarSummary::standardDeviation);

    py::class_<feast::ScalarFieldData>(m, "ScalarFieldData")
        .def(py::init<>())
        .def_readwrite("values", &feast::ScalarFieldData::values)
        .def_readwrite("summary", &feast::ScalarFieldData::summary);

    py::class_<feast::VectorFieldData>(m, "VectorFieldData")
        .def(py::init<>())
        .def_readwrite("values", &feast::VectorFieldData::values)
        .def_readwrite("x", &feast::VectorFieldData::x)
        .def_readwrite("y", &feast::VectorFieldData::y)
        .def_readwrite("z", &feast::VectorFieldData::z)
        .def_readwrite("magnitude", &feast::VectorFieldData::magnitude);

    py::class_<feast::TensorFieldData>(m, "TensorFieldData")
        .def(py::init<>())
        .def_readwrite("values", &feast::TensorFieldData::values)
        .def_readwrite("xx", &feast::TensorFieldData::xx)
        .def_readwrite("yy", &feast::TensorFieldData::yy)
        .def_readwrite("zz", &feast::TensorFieldData::zz)
        .def_readwrite("xy", &feast::TensorFieldData::xy)
        .def_readwrite("yz", &feast::TensorFieldData::yz)
        .def_readwrite("zx", &feast::TensorFieldData::zx)
        .def_readwrite("magnitude", &feast::TensorFieldData::magnitude);

    py::class_<feast::PrincipalFieldData>(m, "PrincipalFieldData")
        .def(py::init<>())
        .def_readwrite("values", &feast::PrincipalFieldData::values)
        .def_readwrite("minimum", &feast::PrincipalFieldData::minimum)
        .def_readwrite("middle", &feast::PrincipalFieldData::middle)
        .def_readwrite("maximum", &feast::PrincipalFieldData::maximum);

    py::class_<feast::StressFieldData>(m, "StressFieldData")
        .def(py::init<>())
        .def_readwrite("values", &feast::StressFieldData::values)
        .def_readwrite("xx", &feast::StressFieldData::xx)
        .def_readwrite("yy", &feast::StressFieldData::yy)
        .def_readwrite("zz", &feast::StressFieldData::zz)
        .def_readwrite("xy", &feast::StressFieldData::xy)
        .def_readwrite("yz", &feast::StressFieldData::yz)
        .def_readwrite("zx", &feast::StressFieldData::zx)
        .def_readwrite("magnitude", &feast::StressFieldData::magnitude)
        .def_readwrite("hydrostatic", &feast::StressFieldData::hydrostatic)
        .def_readwrite("vonMises", &feast::StressFieldData::vonMises)
        .def_readwrite("principal", &feast::StressFieldData::principal);

    py::class_<feast::PostProcessResult>(m, "PostProcessResult")
        .def(py::init<>())
        .def_readwrite("displacement", &feast::PostProcessResult::displacement)
        .def_readwrite("strain", &feast::PostProcessResult::strain)
        .def_readwrite("stress", &feast::PostProcessResult::stress);

    py::class_<feast::PostProcessor>(m, "PostProcessor")
        .def_static("process", &feast::PostProcessor::process,
                    py::arg("result"),
                    py::arg("mesh"),
                    py::arg("dofMap"),
                    py::arg("materials"));
}
