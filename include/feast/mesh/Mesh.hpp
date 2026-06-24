#pragma once

#include <vector>
#include "feast/mesh/Node.hpp"
#include "feast/mesh/Element.hpp"

namespace feast {

class Mesh {
public:
    void addNode(const Node& node);
    void addElement(const Element& element);

    const std::vector<Node>& nodes() const;
    const std::vector<Element>& elements() const;

    void validate() const;

private:
    std::vector<Node> nodes_;
    std::vector<Element> elements_;
};

}
