#include "Quadtree.hpp"

Quadtree::Quadtree(const Bounds& bounds, unsigned int capacity) :
	bounds(bounds), capacity(capacity), divided(false)
{}

void Quadtree::insert(long long id, TreeEdge* edge) {
    // Determine the bounding box for the edge
    Bounds edge_bounds = {
        std::min(edge->v1.position.x, edge->v2.position.x),
        std::min(edge->v1.position.y, edge->v2.position.y),
        std::max(edge->v1.position.x, edge->v2.position.x),
        std::max(edge->v1.position.y, edge->v2.position.y)
    };

    if (!intersects(bounds, edge_bounds)) {
        return;
    }

    if (edges.size() < capacity) {
        edges[id] = edge;
    }
    else {
        if (!divided) {
            subdivide();
        }
        for (auto& child : children) {
            child->insert(id, edge);
        }
    }
}

void Quadtree::query(const Bounds& queryBounds, std::vector<TreeEdge*>& result) const {
    if (!intersects(bounds, queryBounds)) {
        return;
    }

    // Add edges within this node
    for (const auto& [id, edge] : edges) {
        if (intersects(queryBounds, *edge)) {
            result.push_back(edge);
        }
    }

    if (divided) {
        for (const auto& child : children) {
            child->query(queryBounds, result);
        }
    }
}

void Quadtree::rescale(long long id, sf::Vector2f new_pos1, sf::Vector2f new_pos2) {
    auto it = edges.find(id);
    if (it == edges.end()) return; // Make sure id exists
    it->second->v1.position = new_pos1;
    it->second->v2.position = new_pos2;
}

void Quadtree::updateBounds(float window_width, float window_height) {
    std::cout << bounds.right << " " << bounds.bottom << "\n";
    bounds.right = window_width;
    bounds.bottom = window_height;
    std::cout << bounds.right << " " << bounds.bottom << std::endl;
}

void Quadtree::subdivide() {
    float midX = (bounds.left + bounds.right) / 2;
    float midY = (bounds.top + bounds.bottom) / 2;

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
    return intersects(a, Bounds{ edge.v1.position.x, edge.v1.position.y, edge.v2.position.x, edge.v2.position.y });
}

