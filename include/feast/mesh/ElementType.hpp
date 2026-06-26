#pragma once

#include <string>

namespace feast {

enum class ElementType {
    Tet4
};

std::string toString(ElementType type);

}
