#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <iostream>

// Forward declare Graphics
class Graphics;

struct TreeEdge {
	long long id;

	// Store map coordinates
	double lat1, lon1;
	double lat2, lon2;

	// Edge endpoints
	sf::Vertex v1;
	sf::Vertex v2;
};

class Quadtree {
public:
	struct Bounds {
		float left, top, right, bottom;
	};

public:
	Quadtree(const Bounds& bounds, unsigned int capacity = 4);

	// Insert an edge pointer into the quadtree
	void insert(long long id, TreeEdge* edge);

	// Query the quadtree for all edges within the view's bounds
	void query(const Bounds& queryBounds, std::vector<TreeEdge*>& result) const;

private:
	Bounds bounds;
	unsigned int capacity;
	bool divided;
	std::unordered_map<long long, TreeEdge*> edges;
	std::array<std::unique_ptr<Quadtree>, 4> children;

private:
	// Subdivide the current quadtree node into four quadrants
	void subdivide();

	// Check if two bounding boxes intersect
	bool intersects(const Bounds& a, const Bounds& b) const;
	bool intersects(const Bounds& a, const TreeEdge& edge) const; // Overload
};