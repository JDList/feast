#pragma once

#include <cstddef>

namespace feast {

enum class MaterialType {
    LinearElastic
};

class Material {
public:
    virtual ~Material() = default;

    virtual MaterialType type() const = 0;
};

}
