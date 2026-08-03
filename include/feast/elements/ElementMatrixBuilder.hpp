#pragma once

#include <vector>
#include "feast/linalg/DenseMatrix.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/mesh/Mesh.hpp"


namespace feast{

    class ElementMatrixBuilder{
        public:
            static std::vector<DenseMatrix> buildStiffnesses(
                    const Mesh& mesh,
                    const std::vector<LinearElastic>& materials);


            static std::vector<DenseMatrix> buildStiffnesses(
                    const Mesh& mesh,
                    const LinearElastic& material);

    };
}
