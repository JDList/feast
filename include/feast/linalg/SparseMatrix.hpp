#pragma once

#include <Eigen/Sparse>
#include <cstddef>
#include <vector>

namespace feast {

class SparseMatrix {
public:

    SparseMatrix();

    SparseMatrix(std::size_t rows,
                 std::size_t cols);

    std::size_t rows() const;
    std::size_t cols() const;

    void resize(std::size_t rows,
                std::size_t cols);

    void reserve(std::size_t nnz);

    void setZero();

    void setFromTriplets(
        const std::vector<Eigen::Triplet<double>>& triplets
    );
        

    Eigen::SparseMatrix<double>& eigen();
    const Eigen::SparseMatrix<double>& eigen() const;

private:

    Eigen::SparseMatrix<double> data_;
};

}
