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
	void rescaleNodes(float new_width, float new_height);

private:
	// Generate graph edges and insert to quadtree
	void generateEdges();

	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Get the view bounds for current view as Bounds-struct
	Quadtree::Bounds getViewBounds(const sf::View& view);

private:
	Graph& graph;
	std::unique_ptr<Quadtree> quadtree;

	// Store visible edges that get rendered in VertexArray as Lines
	sf::VertexArray visible_edges;

	// Store all the graph edges as TreeEdge-structs that get used in Quadtree
	std::vector<std::unique_ptr<TreeEdge>> graph_edges;

	float window_width;
	float window_height;
};