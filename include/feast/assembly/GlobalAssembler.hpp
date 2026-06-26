#pragma once

#include <cstddef>
#include <vector>

#include "feast/assembly/ElementAssembler.hpp"
#include "feast/bcs/BoundaryConditions.hpp"
#include "feast/dof/DofMap.hpp"
#include "feast/linalg/DenseMatrix.hpp"
#include "feast/linalg/SparseMatrix.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/mesh/Mesh.hpp"

namespace feast {

struct AssemblyResult
{
    SparseMatrix stiffness;
    Vector force;
};

class GlobalAssembler
{
public:
    explicit GlobalAssembler(std::size_t numDofs);

    AssemblyResult assembleLinearSystem(const Mesh& mesh,
                                       const DofMap& dofMap,
                                       const BoundaryConditionSet& boundaryConditions,
                                       const std::vector<DenseMatrix>& elementStiffnesses,
                                       const std::vector<Vector>& elementVectors = {}) const;

private:
    std::size_t m_numDofs{0};
};

} // namespace feast
