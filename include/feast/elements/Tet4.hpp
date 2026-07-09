#pragma once

#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/mesh/Element.hpp"
#include "feast/mesh/Mesh.hpp"

namespace feast {

class Tet4 {
public:
    static DenseMatrix stiffnessMatrix(
        const Mesh& mesh,
        const Element& element,
        const LinearElastic& material);
    static Vector strain(
        const Mesh& mesh,
        const Element& element,
        const Vector& elementDisplacements);

    static Vector stress(
        const Mesh& mesh,
        const Element& element,
        const LinearElastic& material,
        const Vector& elementDisplacements);
};

}
