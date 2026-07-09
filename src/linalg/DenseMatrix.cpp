#include "feast/linalg/DenseMatrix.hpp"

namespace feast {

DenseMatrix::DenseMatrix() = default;

DenseMatrix::DenseMatrix(std::size_t rows, std::size_t cols)
    : data_(rows, cols)
{
}

std::size_t DenseMatrix::rows() const
{
    return data_.rows();
}

std::size_t DenseMatrix::cols() const
{
    return data_.cols();
}

void DenseMatrix::resize(std::size_t rows, std::size_t cols)
{
    data_.resize(rows, cols);
}

void DenseMatrix::setZero()
{
    data_.setZero();
}

double& DenseMatrix::operator()(std::size_t i, std::size_t j)
{
    return data_(i, j);
}

const double& DenseMatrix::operator()(std::size_t i, std::size_t j) const
{
    return data_(i, j);
}

DenseMatrix DenseMatrix::transpose() const
{
    DenseMatrix result;
    result.data_ = data_.transpose();
    return result;
}

DenseMatrix DenseMatrix::operator*(
    const DenseMatrix& other) const
{
    DenseMatrix result;
    result.data_ = data_ * other.data_;
    return result;
}

DenseMatrix& DenseMatrix::operator*=(
    double scalar)
{
    data_ *= scalar;
    return *this;
}

DenseMatrix& DenseMatrix::operator+=(
    const DenseMatrix& other)
{
    data_ += other.data_;
    return *this;
}

Vector DenseMatrix::operator*(const Vector& other) const
{
    if (cols() != other.size()) {
        throw std::invalid_argument("DenseMatrix::operator*: dimension mismatch.");
    }

    Vector result;
    result.resize(rows());
    result.eigen() = data_ * other.eigen();
    return result;
}

void DenseMatrix::setConstant(double value)
{
    data_.setConstant(value);
}

void DenseMatrix::setIdentity()
{
    data_.setIdentity();
}

Eigen::MatrixXd& DenseMatrix::eigen()
{
    return data_;
}

const Eigen::MatrixXd& DenseMatrix::eigen() const
{
    return data_;
}

}
