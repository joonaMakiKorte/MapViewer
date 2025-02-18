#include "Quadtree.hpp"

Quadtree::Quadtree(const Bounds& bounds, unsigned int capacity) :
	bounds(bounds), capacity(capacity), divided(false)
{}

void Quadtree::insert(const sf::Vertex & v1, const sf::Vertex & v2) {

}

void Quadtree::query(const Bounds& queryBounds, std::vector<Edge>& result) const {

}

void Quadtree::subdivide() {

}

bool Quadtree::intersects(const Bounds& a, const Bounds& b) const {
	return false;
}

