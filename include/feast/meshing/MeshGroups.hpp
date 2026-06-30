#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace feast{

    struct BoundaryFace{

        std::vector<std::size_t> node_ids;
    };
    struct MeshGroups{

        std::map<std::string, std::vector<std::size_t>> node_sets;
        std::map<std::string, std::vector<BoundaryFace>> face_sets;

        void clear(){

            node_sets.clear();
            face_sets.clear();

        };

    };

}

