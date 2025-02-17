#include "Graphics.hpp"

Graphics::Graphics(Graph& graph) : 
	graph(graph), graph_edges(sf::PrimitiveType::Lines), window_width(1024), window_height(768) {
	generateEdges();
}

void Graphics::render(sf::RenderWindow & window) {
	window.draw(graph_edges);
}

sf::Vector2f Graphics::transformToSFML(double lat, double lon) {
	// Prevent division by zero
	double lat_range = std::max(graph.bbox.max_lat - graph.bbox.min_lat, 1e-6);
	double lon_range = std::max(graph.bbox.max_lon - graph.bbox.min_lon, 1e-6);

	// Calculate normalized latitude and longitude for correct graphics coordinates
	double normalized_y = (graph.bbox.max_lat - lat) / lat_range;
	double normalized_x = (lon - graph.bbox.min_lon) / lon_range;

	// Scale to window size
	float x = static_cast<float>(normalized_x * window_width);
	float y = static_cast<float>(normalized_y * window_height);

	return sf::Vector2f(x, y);
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of edges * 2
	const std::unordered_map<long long, Node>& nodes = graph.getNodes();
	const std::vector<Edge>& edges = graph.getEdges();
	std::cout << edges.size() << " " << nodes.size() << std::endl;
	graph_edges.resize(edges.size()*2);

	// Iterate over edges and create lines
	// Transform each node to SFML
	std::size_t idx = 0;
	for (const auto& edge : edges) {
		const Node& from = nodes.at(edge.from);
		const Node& target = nodes.at(edge.to);

		graph_edges[idx++].position = transformToSFML(from.lat, from.lon);
		graph_edges[idx++].position = transformToSFML(target.lat, target.lon);
	}
}


