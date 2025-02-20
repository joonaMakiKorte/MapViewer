#pragma once

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "Quadtree.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

class Graphics
{
public:
	// Constructor
	Graphics(Graph& graph, float window_width, float window_height);

	// Render map, aka display graph edges
	void render(sf::RenderWindow& window, const sf::View& view);

	// Change edge color by ID
	void changeEdgeColor(long long id, sf::Color new_color);

	// When window gets resized, rescale nodes
	void rescaleGraphics(float new_width, float new_height);

	// Handle selecting a node by mouse click
	void selectNode(sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mouse_pos);

private:
	// Generate graph edges and insert to quadtree
	void generateEdges();

	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Get the view bounds for current view as Bounds-struct
	Quadtree::Bounds getViewBounds(const sf::View& view);

	// Get the node closest to the given world position
	// Returns a pointer to the closest node, nullptr if not found
	// Takes a vector of TreeEdge-pointers to search from
	sf::Vertex* getClosestNode(const sf::Vector2f& world_pos, const std::vector<TreeEdge*>& edges);

	// Calculate the Euclidean distance between two points
	float distance(const sf::Vector2f& p1, const sf::Vector2f& p2);

private:
	Graph& graph;
	std::unique_ptr<Quadtree> quadtree;

	// Store visible edges that get rendered in VertexArray as Lines
	sf::VertexArray visible_edges;

	// Store all the graph edges as TreeEdge-structs that get used in Quadtree
	// Access by ID
	std::unordered_map<long long, std::unique_ptr<TreeEdge>> graph_edges;

	float window_width;
	float window_height;

	// Circle shape for node selection
	sf::CircleShape selection_circle;
};