#pragma once

#include <Eigen/Dense>
#include <cstddef>

namespace feast {

class DenseMatrix {
public:
    DenseMatrix();
    DenseMatrix(std::size_t rows, std::size_t cols);

    std::size_t rows() const;
    std::size_t cols() const;

    void resize(std::size_t rows, std::size_t cols);
    void setZero();

    double& operator()(std::size_t i, std::size_t j);
    const double& operator()(std::size_t i, std::size_t j) const;
    
    DenseMatrix transpose() const;
    DenseMatrix operator*(const DenseMatrix& other) const;
    DenseMatrix& operator*=(double scalar);
    DenseMatrix& operator+=(const DenseMatrix& other);

    void setConstant(double value);
    void SetIdentity();
    
    Eigen::MatrixXd& eigen();
    const Eigen::MatrixXd& eigen() const;

private:
    Eigen::MatrixXd data_;
};

}
