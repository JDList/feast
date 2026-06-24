#include "feast/linalg/SparseMatrix.hpp"

namespace feast {

SparseMatrix::SparseMatrix()
{
}

SparseMatrix::SparseMatrix(
    std::size_t rows,
    std::size_t cols)
    : data_(rows, cols)
{
}

std::size_t SparseMatrix::rows() const
{
    return data_.rows();
}

std::size_t SparseMatrix::cols() const
{
    return data_.cols();
}

void SparseMatrix::resize(
    std::size_t rows,
    std::size_t cols)
{
    data_.resize(rows, cols);
}

void SparseMatrix::reserve(
    std::size_t nnz)
{
    data_.reserve(nnz);
}

void SparseMatrix::setZero()
{
    data_.setZero();
}

void SparseMatrix::setFromTriplets(
        const std::vector<Eigen::Triplet<double>>& triplets)
{
    data_.setFromTriplets(
            triplets.begin(),
            triplets.end()
    );
}

Eigen::SparseMatrix<double>&
SparseMatrix::eigen()
{
    return data_;
}

const Eigen::SparseMatrix<double>&
SparseMatrix::eigen() const
{
    return data_;
}

}
