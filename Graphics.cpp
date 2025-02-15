#include "Graphics.hpp"

Graphics::Graphics(Graph& graph) :
	graph(graph), window_width(800), window_height(600)
{}

sf::Vector2f Graphics::transformToSFML(double lat, double lon) {
	// Calculate normalized latitude and longitude for correct graphics coordinates
	double normalized_y = (graph.getLimit(true,true) - lat) / (graph.getLimit(true,true) - graph.getLimit(true, false));
	double normalized_x = (lon - graph.getLimit(false,false)) / (graph.getLimit(false,true) - graph.getLimit(false,false));

	// Scale to window size
	float x = static_cast<float>(normalized_x * window_width);
	float y = static_cast<float>(normalized_y * window_height);

	return sf::Vector2f(x, y);
}

