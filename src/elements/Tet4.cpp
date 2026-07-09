#include "feast/elements/Tet4.hpp"

#include <array>
#include <cmath>
#include <stdexcept>

namespace feast {
namespace {

DenseMatrix inverse3x3(const DenseMatrix& A)
{
    if (A.rows() != 3 || A.cols() != 3) {
        throw std::invalid_argument("inverse3x3 requires a 3x3 matrix.");
    }

    const double a = A(0, 0);
    const double b = A(0, 1);
    const double c = A(0, 2);
    const double d = A(1, 0);
    const double e = A(1, 1);
    const double f = A(1, 2);
    const double g = A(2, 0);
    const double h = A(2, 1);
    const double i = A(2, 2);

    const double det =
        a * (e * i - f * h)
      - b * (d * i - f * g)
      + c * (d * h - e * g);

    if (std::abs(det) < 1e-14) {
        throw std::runtime_error("Tet4 Jacobian is singular or nearly singular.");
    }

    DenseMatrix inv(3, 3);

    inv(0, 0) =  (e * i - f * h) / det;
    inv(0, 1) = -(b * i - c * h) / det;
    inv(0, 2) =  (b * f - c * e) / det;

    inv(1, 0) = -(d * i - f * g) / det;
    inv(1, 1) =  (a * i - c * g) / det;
    inv(1, 2) = -(a * f - c * d) / det;

    inv(2, 0) =  (d * h - e * g) / det;
    inv(2, 1) = -(a * h - b * g) / det;
    inv(2, 2) =  (a * e - b * d) / det;

    return inv;
}

DenseMatrix jacobianForTet4(const Mesh& mesh, const Element& element)
{
    const auto& nodes = mesh.nodes();

    const Node& n1 = nodes.at(element.node_ids[0]);
    const Node& n2 = nodes.at(element.node_ids[1]);
    const Node& n3 = nodes.at(element.node_ids[2]);
    const Node& n4 = nodes.at(element.node_ids[3]);

    DenseMatrix J(3, 3);
    J(0, 0) = n2.x - n1.x;
    J(0, 1) = n3.x - n1.x;
    J(0, 2) = n4.x - n1.x;

    J(1, 0) = n2.y - n1.y;
    J(1, 1) = n3.y - n1.y;
    J(1, 2) = n4.y - n1.y;

    J(2, 0) = n2.z - n1.z;
    J(2, 1) = n3.z - n1.z;
    J(2, 2) = n4.z - n1.z;

    return J;
}

double tet4Volume(const DenseMatrix& J)
{
    const double detJ =
        J(0, 0) * (J(1, 1) * J(2, 2) - J(1, 2) * J(2, 1))
      - J(0, 1) * (J(1, 0) * J(2, 2) - J(1, 2) * J(2, 0))
      + J(0, 2) * (J(1, 0) * J(2, 1) - J(1, 1) * J(2, 0));

    return std::abs(detJ) / 6.0;
}

DenseMatrix tet4BMatrix(const Mesh& mesh, const Element& element)
{
    const DenseMatrix J = jacobianForTet4(mesh, element);
    const DenseMatrix Jinv = inverse3x3(J);
    const DenseMatrix JTinv = Jinv.transpose();

    const std::array<std::array<double, 3>, 4> grad_ref = {{
        {{-1.0, -1.0, -1.0}},
        {{ 1.0,  0.0,  0.0}},
        {{ 0.0,  1.0,  0.0}},
        {{ 0.0,  0.0,  1.0}}
    }};

    DenseMatrix B(6, 12);
    B.setZero();

    for (std::size_t a = 0; a < 4; ++a) {
        const double dN_dxi   = grad_ref[a][0];
        const double dN_deta  = grad_ref[a][1];
        const double dN_dzeta = grad_ref[a][2];

        const double dN_dx =
            JTinv(0, 0) * dN_dxi +
            JTinv(0, 1) * dN_deta +
            JTinv(0, 2) * dN_dzeta;

        const double dN_dy =
            JTinv(1, 0) * dN_dxi +
            JTinv(1, 1) * dN_deta +
            JTinv(1, 2) * dN_dzeta;

        const double dN_dz =
            JTinv(2, 0) * dN_dxi +
            JTinv(2, 1) * dN_deta +
            JTinv(2, 2) * dN_dzeta;

        const std::size_t c = 3 * a;

        B(0, c + 0) = dN_dx;
        B(1, c + 1) = dN_dy;
        B(2, c + 2) = dN_dz;

        B(3, c + 0) = dN_dy;
        B(3, c + 1) = dN_dx;

        B(4, c + 1) = dN_dz;
        B(4, c + 2) = dN_dy;

        B(5, c + 0) = dN_dz;
        B(5, c + 2) = dN_dx;
    }

    return B;
}

DenseMatrix tet4ConstitutiveMatrix(const LinearElastic& material)
{
    const double E = material.youngsModulus();
    const double nu = material.poissonRatio();

    const double lambda = (E * nu) / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double mu = E / (2.0 * (1.0 + nu));

    DenseMatrix D(6, 6);
    D.setZero();

    D(0, 0) = lambda + 2.0 * mu;
    D(1, 1) = lambda + 2.0 * mu;
    D(2, 2) = lambda + 2.0 * mu;

    D(0, 1) = lambda;
    D(0, 2) = lambda;
    D(1, 0) = lambda;
    D(1, 2) = lambda;
    D(2, 0) = lambda;
    D(2, 1) = lambda;

    D(3, 3) = mu;
    D(4, 4) = mu;
    D(5, 5) = mu;

    return D;
}

} // namespace

DenseMatrix Tet4::stiffnessMatrix(
    const Mesh& mesh,
    const Element& element,
    const LinearElastic& material)
{
    if (element.type != ElementType::Tet4) {
        throw std::invalid_argument("Tet4::stiffnessMatrix called with non-Tet4 element.");
    }

    if (element.node_ids.size() != 4) {
        throw std::invalid_argument("Tet4 element must have exactly 4 node IDs.");
    }

    const DenseMatrix J = jacobianForTet4(mesh, element);
    const double volume = tet4Volume(J);

    if (volume <= 0.0) {
        throw std::runtime_error("Tet4 element has zero or negative volume.");
    }

    const DenseMatrix B = tet4BMatrix(mesh, element);
    const DenseMatrix D = tet4ConstitutiveMatrix(material);

    DenseMatrix Ke = B.transpose() * D * B;
    Ke *= volume;

    return Ke;
}


Vector Tet4::strain(
    const Mesh& mesh,
    const Element& element,
    const Vector& elementDisplacements)
{
    if (element.type != ElementType::Tet4) {
        throw std::invalid_argument("Tet4::strain called with non-Tet4 element.");
    }

    if (element.node_ids.size() != 4) {
        throw std::invalid_argument("Tet4 element must have exactly 4 node IDs.");
    }

    if (elementDisplacements.size() != 12) {
        throw std::invalid_argument("Tet4 displacement vector must have size 12.");
    }

    const DenseMatrix B = tet4BMatrix(mesh, element);
    Vector eps(6);
    eps = B * elementDisplacements;
    return eps;
}

Vector Tet4::stress(
    const Mesh& mesh,
    const Element& element,
    const LinearElastic& material,
    const Vector& elementDisplacements)
{
    const Vector eps = strain(mesh, element, elementDisplacements);
    const DenseMatrix D = tet4ConstitutiveMatrix(material);

    Vector sigma(6);
    sigma = D * eps;
    return sigma;
}

} // namespace feast
