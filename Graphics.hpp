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
	void render(sf::RenderWindow& window);

	// Change edge color by ID
	void changeEdgeColor(long long id, sf::Color new_color);

	// Get the position of two edge vertices by id
	const std::pair<sf::Vertex, sf::Vertex>& getEdge(long long id) const;

private:
	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Generate visible edges (sf::VertexArray)
	void generateEdges();

	Graph& graph;
	std::unique_ptr<Quadtree> quadtree;

	// Store edges in VertexArray as Lines
	sf::VertexArray graph_edges;

	// edge_id -> (graph_edges[i], graph_edges[i+1])
	std::unordered_map<long long, std::pair<std::size_t, std::size_t>> edge_to_vertex;

	float window_width;
	float window_height;
};