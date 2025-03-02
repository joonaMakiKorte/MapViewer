#ifndef QUADTREE_H
#define	QUADTREE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// Forward declare Graphics
class Graphics;

class Quadtree {
public:
	struct TreeEdge {
		uint32_t id; // Edge id
		sf::Vector2f v1, v2; // Edge endpoints
		sf::Color color; // Color depending on if in path
		float thickness; // Thickess of edge (found path appears thicker)
	};

	struct Bounds {
		float left, top, right, bottom;
	};

public:
	// Quadtree gets constructed with the bounds calculated by the current window resolution
	Quadtree(const Bounds& bounds, unsigned int capacity = 4);

	// Insert an edge pointer into the quadtree
	void insert(TreeEdge* edge);

	// Recursively query the quadtree for all edges within the view's bounds
	// All edges fitting the view's bounds gets inserted to the vector passed as a parameter
	void query(const Bounds& queryBounds, std::vector<TreeEdge*>& result) const;

private:
	Bounds bounds; // Store the bounds of current quadtree for querying
	unsigned int capacity; // Capasity of a node, aka max children count
	bool divided; // Keep track if a node is divided
	std::vector<TreeEdge*> edges; // Store edges of quadtree
	std::array<std::unique_ptr<Quadtree>, 4> children; // Store children as quadtrees

private:
	// Subdivide the current quadtree node into four quadrants
	void subdivide();

	// Check if two bounding boxes intersect
	bool intersects(const Bounds& a, const Bounds& b) const;
	// Overload to comparare if an edge intersects with a bounding box
	bool intersects(const Bounds& a, const TreeEdge& edge) const;
};

#endif