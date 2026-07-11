#include <pybind11/pybind11.h>

namespace py = pybind11;

void bindLinalg(py::module_& m);
void bindMesh(py::module_& m);
void bindGeometry(py::module_& m);
void bindMeshing(py::module_& m);
void bindBoundaryConditions(py::module_& m);
void bindMaterials(py::module_& m);
void bindElements(py::module_& m);
void bindKernel(py::module_& m);
void bindPostProcessing(py::module_& m);
void bindDof(py::module_& m);

PYBIND11_MODULE(_core, m)
{
    m.doc() = "FEAST Python bindings";

    bindLinalg(m);
    bindMesh(m);
    bindGeometry(m);
    bindMeshing(m);
    bindBoundaryConditions(m);
    bindMaterials(m);
    bindElements(m);
    bindKernel(m);
    bindPostProcessing(m);
    bindDof(m);
}
