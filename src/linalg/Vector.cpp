#include "feast/linalg/Vector.hpp"

namespace feast {

Vector::Vector(std::size_t size)
    : data_(size)
{
    data_.setZero();
}

std::size_t Vector::size() const {
    return data_.size();
}

double& Vector::operator[](std::size_t i) {
    return data_(i);
}

const double& Vector::operator[](std::size_t i) const {
    return data_(i);
}

void Vector::resize(std::size_t n) {
    data_.resize(n);
}

void Vector::setZero() {
    data_.setZero();
}

void Vector::setConstant(double value)
{
    data_.setConstant(value);
}

double Vector::norm() const
{
    return data_.norm();
}

Eigen::VectorXd& Vector::eigen() {
    return data_;
}

const Eigen::VectorXd& Vector::eigen() const {
    return data_;
}

}
