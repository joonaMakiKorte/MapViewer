#include "Graphics.hpp"

Graphics::Graphics(Graph& graph) :
	graph(graph), window_width(800), window_height(600)
{}

sf::Vector2f Graphics::transformToSFML(double lat, double lon) {
	// Calculate normalized latitude and longitude for correct graphics coordinates
	double normalized_y = (graph.bbox.max_lat - lat) / (graph.bbox.max_lat - graph.bbox.min_lat);
	double normalized_x = (lon - graph.bbox.min_lon) / (graph.bbox.max_lon - graph.bbox.min_lon);

	// Scale to window size
	float x = static_cast<float>(normalized_x * window_width);
	float y = static_cast<float>(normalized_y * window_height);

	return sf::Vector2f(x, y);
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of nodes * 2
	const std::unordered_map<long long, Node>& nodes = graph.getNodes();
	const std::vector<Edge>& edges = graph.getEdges();
	graph_edges.resize(nodes.size());

	// Iterate over edges and create lines
	// Transform each node to SFML
	std::size_t idx = 0;
	for (const auto& edge : edges) {
		// Get source node and add as vertex
		const Node& from = nodes.at(edge.from);
		graph_edges[idx++].position = transformToSFML(from.lat, from.lon);

		// Same for target node
		const Node& target = nodes.at(edge.to);
		graph_edges[idx++].position = transformToSFML(target.lat, target.lon);
	}
}


