#include "Quadtree.hpp"

Quadtree::Quadtree(const Bounds& bounds, unsigned int capacity) :
	bounds(bounds), capacity(capacity), divided(false)
{}

void Quadtree::insert(TreeEdge* edge) {
    // Determine the bounding box for the edge
    Bounds edge_bounds = {
        std::min(edge->v1.x, edge->v2.x),
        std::min(edge->v1.y, edge->v2.y),
        std::max(edge->v1.x, edge->v2.x),
        std::max(edge->v1.y, edge->v2.y)
    };

    // Cannot insert if the edge bounds don't fit the view
    if (!intersects(bounds, edge_bounds)) {
        return;
    }

    // Add edges to current node if capacity hasn't been reached
    // Else subdivide current node to four quadrants and create children
    if (edges.size() < capacity) {
        edges.push_back(edge);
    }
    else {
        if (!divided) {
            subdivide();
        }
        for (auto& child : children) {
            child->insert(edge);
        }
    }
}

void Quadtree::query(const Bounds& queryBounds, std::vector<TreeEdge*>& result) const {
    // Return condition for recursion
    if (!intersects(bounds, queryBounds)) {
        return;
    }

    // Add edges within this node
    for (const auto& edge : edges) {
        if (intersects(queryBounds, *edge)) {
            result.push_back(edge);
        }
    }

    // If the node is divided query the children withing bounds
    if (divided) {
        for (const auto& child : children) {
            child->query(queryBounds, result);
        }
    }
}

void Quadtree::subdivide() {
    // Calculate limits for quadrant bounding boxes
    float midX = (bounds.left + bounds.right) / 2;
    float midY = (bounds.top + bounds.bottom) / 2;

    // Subdivide current bounding box to four quadrants with equal sizes
    children[0] = std::make_unique<Quadtree>(Bounds{ bounds.left, bounds.top, midX, midY });
    children[1] = std::make_unique<Quadtree>(Bounds{ midX, bounds.top, bounds.right, midY });
    children[2] = std::make_unique<Quadtree>(Bounds{ bounds.left, midY, midX, bounds.bottom });
    children[3] = std::make_unique<Quadtree>(Bounds{ midX, midY, bounds.right, bounds.bottom });

    divided = true;
}

bool Quadtree::intersects(const Bounds& a, const Bounds& b) const {
    return !(a.right < b.left || a.left > b.right || a.bottom < b.top || a.top > b.bottom);
}

bool Quadtree::intersects(const Bounds& a, const TreeEdge& edge) const {
    return intersects(a, Bounds{ edge.v1.x, edge.v1.y, edge.v2.x, edge.v2.y });
}

