#pragma once

#include <cstddef>
#include <vector>

#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/linalg/Triplet.hpp"

namespace feast {


class ElementAssembler
{
public:
    static void addStiffnessTriplets(const std::vector<int>& dofIds,
                                     const DenseMatrix& elementStiffness,
                                     std::vector<Triplet>& triplets);

    static void addVectorContribution(const std::vector<int>& dofIds,
                                      const Vector& elementVector,
                                      Vector& globalVector);
};

} // namespace feast
