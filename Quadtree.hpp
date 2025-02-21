#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <iostream>

// Forward declare Graphics
class Graphics;

struct TreeEdge {
	// Edge endpoints
	sf::Vertex v1;
	sf::Vertex v2;

	// Endpoint node IDs
	int64_t v1_id;
	int64_t v2_id;
};

class Quadtree {
public:
	struct Bounds {
		float left, top, right, bottom;
	};

public:
	Quadtree(const Bounds& bounds, unsigned int capacity = 4);

	// Insert an edge pointer into the quadtree
	void insert(TreeEdge* edge);

	// Query the quadtree for all edges within the view's bounds
	void query(const Bounds& queryBounds, std::vector<TreeEdge*>& result) const;

private:
	Bounds bounds;
	unsigned int capacity;
	bool divided;
	std::vector<TreeEdge*> edges;
	std::array<std::unique_ptr<Quadtree>, 4> children;

private:
	// Subdivide the current quadtree node into four quadrants
	void subdivide();

	// Check if two bounding boxes intersect
	bool intersects(const Bounds& a, const Bounds& b) const;
	bool intersects(const Bounds& a, const TreeEdge& edge) const; // Overload
};