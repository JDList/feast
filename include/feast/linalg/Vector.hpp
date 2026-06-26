#pragma once

#include <Eigen/Dense>
#include <cstddef>

namespace feast {

class Vector {
public:

    Vector() = default;

    explicit Vector(std::size_t size);

    std::size_t size() const;

    double& operator[](std::size_t i);
    const double& operator[](std::size_t i) const;

    void resize(std::size_t n);

    void setZero();
    void setConstant(double value);
    double norm() const;


    Eigen::VectorXd& eigen();
    const Eigen::VectorXd& eigen() const;

private:
    Eigen::VectorXd data_;
};

}
