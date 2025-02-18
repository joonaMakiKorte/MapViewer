#pragma once

#include <SFML/Graphics.hpp>
#include "Graphics.hpp"
#include <vector>
#include <memory>

struct TreeEdge {
	sf::Vertex v1;
	sf::Vertex v2;
};

class Quadtree {
public:
	struct Bounds {
		float left, top, right, bottom;
	};

	Quadtree(const Bounds& bounds, unsigned int capacity = 4);

	// Insert an edge into the quadtree using coordinates from Graphics
	void insert(const sf::Vertex& v1, const sf::Vertex& v2);

	// Query the quadtree for all edges within the view's bounds
	void query(const Bounds& queryBounds, std::vector<Edge>& result) const;

private:
	Bounds bounds;
	unsigned int capacity;
	bool divided;
	std::vector<TreeEdge> edges;
	std::array<std::unique_ptr<Quadtree>, 4> children;

	// Subdivide the current quadtree node into four quadrants
	void subdivide();

	// Check if two bounding boxes intersect
	bool intersects(const Bounds& a, const Bounds& b) const;
};