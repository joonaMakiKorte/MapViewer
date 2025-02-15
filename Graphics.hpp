#pragma once

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include <vector>

class Graphics
{
public:
	// Constructor
	Graphics(Graph& graph);

	// Render map, aka display graph edges
	void render(sf::RenderWindow& window);

private:
	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Generate visible edges (sf::VertexArray)
	void generateEdges();

	Graph& graph;
	std::vector<sf::VertexArray> edges;
	float window_width;
	float window_height;
};