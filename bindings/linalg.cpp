#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Triplet.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/linalg/EigenDirectSolver.hpp"


namespace py = pybind11;

void bindLinalg(py::module_& m)
{
    py::class_<feast::Vector>(m, "Vector")
        .def(py::init<>())
        .def("resize", &feast::Vector::resize)
        .def("size", &feast::Vector::size)
        .def("setZero", &feast::Vector::setZero)
        .def("setConstant", &feast::Vector::setConstant)
        .def("norm", &feast::Vector::norm)
        .def("__len__", &feast::Vector::size)
        .def("__getitem__", [](const feast::Vector& v, std::size_t i) {
            return v[i];
        })
        .def("__setitem__", [](feast::Vector& v, std::size_t i, double value) {
            v[i] = value;
        })
        .def("__repr__", [](const feast::Vector& v) {
            return "<feast.Vector size=" + std::to_string(v.size()) + ">";
        });

    py::class_<feast::DenseMatrix>(m, "DenseMatrix")
        .def(py::init<>())
        .def("resize", &feast::DenseMatrix::resize)
        .def("rows", &feast::DenseMatrix::rows)
        .def("cols", &feast::DenseMatrix::cols)
        .def("setZero", &feast::DenseMatrix::setZero)
        .def("setConstant", &feast::DenseMatrix::setConstant)
        .def("setIdentity", &feast::DenseMatrix::setIdentity)
        .def("transpose", &feast::DenseMatrix::transpose)
        .def("__mul__", [](const feast::DenseMatrix& m, const feast::Vector& v) {
            return m * v;
        })
        .def("__getitem__", [](const feast::DenseMatrix& m, std::pair<std::size_t, std::size_t> ij) {
            return m(ij.first, ij.second);
        })
        .def("__setitem__", [](feast::DenseMatrix& m, std::pair<std::size_t, std::size_t> ij, double value) {
            m(ij.first, ij.second) = value;
        })
        .def("__repr__", [](const feast::DenseMatrix& m) {
            return "<feast.DenseMatrix " + std::to_string(m.rows()) + "x" + std::to_string(m.cols()) + ">";
        });

    py::class_<feast::Triplet>(m, "Triplet")
        .def(py::init<>())
        .def(py::init<std::size_t, std::size_t, double>(),
             py::arg("row"), py::arg("col"), py::arg("value"))
        .def_readwrite("row", &feast::Triplet::row)
        .def_readwrite("col", &feast::Triplet::col)
        .def_readwrite("value", &feast::Triplet::value);

    py::class_<feast::SparseMatrix>(m, "SparseMatrix")
        .def(py::init<>())
        .def("resize", &feast::SparseMatrix::resize)
        .def("reserve", &feast::SparseMatrix::reserve)
        .def("rows", &feast::SparseMatrix::rows)
        .def("cols", &feast::SparseMatrix::cols)
        .def("setZero", &feast::SparseMatrix::setZero)
        .def("setFromTriplets", &feast::SparseMatrix::setFromTriplets);

    py::class_<feast::LinearSolver>(m, "LinearSolver");

    py::class_<feast::EigenDirectSolver, feast::LinearSolver>(m, "EigenDirectSolver")
        .def(py::init<>());
}
