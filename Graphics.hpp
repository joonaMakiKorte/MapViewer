#pragma once

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "Quadtree.hpp"
#include "Algorithm.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>

constexpr int64_t UNASSIGNED = -1; // Sentinel value for unassigned node ID

class Graphics
{
public:
	// Constructor
	Graphics(Graph& graph, float window_width, float window_height);

	// Render map, aka display graph edges
	void render(sf::RenderWindow& window, const sf::View& view);

	// Change edge color by ID
	void changeEdgeColor(uint32_t id, sf::Color new_color);

	// When window gets resized, rescale nodes
	void rescaleGraphics(float new_width, float new_height);

	// Handle selecting a node by mouse click
	void selectNode(sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mouse_pos);

	// Handle finding a route between two nodes, return boolean indicating if can be executed
	// If so, call Algorithm::runDijkstra to find the shortest path and highlight path
	// If no path is found, distance reference remains zero 
	bool findRoute(double& distance);

private:
	// Generate graph edges and insert to quadtree
	void generateEdges();

	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Get the view bounds for current view as Bounds-struct
	Quadtree::Bounds getViewBounds(const sf::View& view);

	// Get the node closest to the given world position
	// Returns a pointer to the closest node and modifies reference id, nullptr if not found
	// Takes a vector of TreeEdge-pointers to search from
	sf::Vertex* getClosestNode(const sf::Vector2f& world_pos, const std::vector<TreeEdge*>& edges, int64_t& selected_id);

	// Calculate the Euclidean distance between two points
	float distance(const sf::Vector2f& p1, const sf::Vector2f& p2);

	// Highlight the path between two nodes
	void highlightPath(sf::Color new_color);

private:
	Graph& graph;
	std::unique_ptr<Quadtree> quadtree;

	// Store visible edges that get rendered in VertexArray as Lines
	sf::VertexArray visible_edges;

	// Store all the graph edges as TreeEdge-structs that get used in Quadtree
	// Access by ID
	std::unordered_map<uint32_t, std::unique_ptr<TreeEdge>> graph_edges;

	float window_width;
	float window_height;

	// Circle shape for node selection
	sf::CircleShape from_circle;
	sf::CircleShape target_circle;

	// Track selected nodes by id
	int64_t from_id;
	int64_t target_id;

	// Track previous found path
	std::vector<uint32_t> current_path;

};