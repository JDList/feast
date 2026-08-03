#include "feast/elements/ElementMatrixBuilder.hpp"
#include "feast/elements/Tet4.hpp"

#include <vector>
#include <stdexcept>

namespace feast {

    std::vector<DenseMatrix> ElementMatrixBuilder::buildStiffnesses(
            const Mesh& mesh,
            const std::vector<LinearElastic>& materials){

        const auto& elements = mesh.elements();

        std::vector<DenseMatrix> result;
        result.reserve(elements.size());

        for (const Element& element : elements){
            if (element.material_id >= materials.size()){
                throw std::out_of_range("Element material_id is outside range of materials table");
            }
            const LinearElastic& material = materials[element.material_id];

            switch (element.type){

                case ElementType::Tet4:
                    result.emplace_back(Tet4::stiffnessMatrix(mesh, element, material));
                    break;

                default:
                    throw std::runtime_error("Unsupported element Type.");


            }
        }

        return result;

}
    std::vector<DenseMatrix> ElementMatrixBuilder::buildStiffnesses(
            const Mesh& mesh,
            const LinearElastic& material){

        const auto& elements = mesh.elements();

        std::vector<DenseMatrix> result;
        result.reserve(elements.size());

        for (const Element& element : elements){

            switch (element.type){

                case ElementType::Tet4:
                    result.emplace_back(Tet4::stiffnessMatrix(mesh, element, material));
                    break;

                default:
                    throw std::runtime_error("Unsupported element Type.");


            }
        }

        return result;

}
}
