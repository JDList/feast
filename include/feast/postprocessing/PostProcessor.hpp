#pragma once

#include "feast/dof/DofMap.hpp"
#include "feast/kernel/Kernel.hpp"
#include "feast/linalg/Vector.hpp"
#include "feast/materials/LinearElastic.hpp"
#include "feast/mesh/Element.hpp"
#include "feast/mesh/Mesh.hpp"
#include "feast/postprocessing/PostProcessingTypes.hpp"

#include <cstddef>
#include <vector>

namespace feast {

class PostProcessor
{
public:
    PostProcessor() = delete;

    static ScalarSummary summarize(const std::vector<double>& values);

    static double magnitude(const Vector3& value);
    static std::vector<double> magnitudes(const std::vector<Vector3>& values);

    static double vonMises(const Voigt6& stress);
    static std::vector<double> vonMises(const std::vector<Voigt6>& stresses);

    static double hydrostaticStress(const Voigt6& stress);
    static std::vector<double> hydrostaticStress(const std::vector<Voigt6>& stresses);

    static double deviatoricJ2(const Voigt6& stress);
    static std::vector<double> deviatoricJ2(const std::vector<Voigt6>& stresses);

    static Principal3 principalStresses(const Voigt6& stress);
    static std::vector<Principal3> principalStresses(const std::vector<Voigt6>& stresses);

    static PostProcessResult process(
        const LinearStaticResult& result,
        const Mesh& mesh,
        const DofMap& dofMap,
        const std::vector<LinearElastic>& materials);

private:
    static std::size_t resolveDofIndex(
        const DofMap& dofMap,
        std::size_t nodeId,
        std::size_t dof);

    static Vector recoverTet4ElementDisplacements(
        const LinearStaticResult& result,
        const DofMap& dofMap,
        const Element& element);

    static void populateVectorField(
        VectorFieldData& field,
        const std::vector<Vector3>& values);

    static void populateTensorField(
        TensorFieldData& field,
        const std::vector<Voigt6>& values);

    static void populateStressField(
        StressFieldData& field,
        const std::vector<Voigt6>& values);
};

} // namespace feast
