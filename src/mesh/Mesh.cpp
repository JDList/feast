#include "feast/mesh/Mesh.hpp"

#include <stdexcept>

namespace feast {

void Mesh::addNode(const Node& node)
{
    nodes_.push_back(node);
}

void Mesh::addElement(const Element& element)
{
    elements_.push_back(element);
}

const std::vector<Node>& Mesh::nodes() const
{
    return nodes_;
}

const std::vector<Element>& Mesh::elements() const
{
    return elements_;
}

void Mesh::validate() const
{
    if (nodes_.empty()) {
        throw std::runtime_error("Mesh validation failed: no nodes in mesh.");
    }

    if (elements_.empty()) {
        throw std::runtime_error("Mesh validation failed: no elements in mesh.");
    }

    for (const auto& element : elements_) {
        for (std::size_t node_id : element.node_ids) {
            if (node_id >= nodes_.size()) {
                throw std::runtime_error("Mesh validation failed: element references invalid node id.");
            }
        }
    }
}

}
