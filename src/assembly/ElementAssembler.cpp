#include "feast/assembly/ElementAssembler.hpp"

#include <stdexcept>

namespace feast {

void ElementAssembler::addStiffnessTriplets(const std::vector<int>& dofIds,
                                             const DenseMatrix& elementStiffness,
                                             std::vector<Triplet>& triplets)
{
    const std::size_t n = dofIds.size();

    if (elementStiffness.rows() != n || elementStiffness.cols() != n)
        throw std::invalid_argument("Element stiffness size does not match DOF count.");

    triplets.reserve(triplets.size() + n * n);

    for (std::size_t i = 0; i < n; ++i)
    {
        if (dofIds[i] < 0)
            throw std::invalid_argument("Negative global DOF index encountered.");

        for (std::size_t j = 0; j < n; ++j)
        {
            if (dofIds[j] < 0)
                throw std::invalid_argument("Negative global DOF index encountered.");

            const double value = elementStiffness(i, j);
            if (value != 0.0)
            {
                triplets.push_back(Triplet{
                    static_cast<std::size_t>(dofIds[i]),
                    static_cast<std::size_t>(dofIds[j]),
                    value
                });
            }
        }
    }
}

void ElementAssembler::addVectorContribution(const std::vector<int>& dofIds,
                                             const Vector& elementVector,
                                             Vector& globalVector)
{
    const std::size_t n = dofIds.size();

    if (elementVector.size() != n)
        throw std::invalid_argument("Element vector size does not match DOF count.");

    if (globalVector.size() == 0)
        throw std::invalid_argument("Global vector has not been sized.");

    for (std::size_t i = 0; i < n; ++i)
    {
        if (dofIds[i] < 0)
            throw std::invalid_argument("Negative global DOF index encountered.");

        globalVector[static_cast<std::size_t>(dofIds[i])] += elementVector[i];
    }
}

} // namespace feast
