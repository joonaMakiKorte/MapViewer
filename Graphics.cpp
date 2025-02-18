#include "Graphics.hpp"

Graphics::Graphics(Graph& graph, float window_width, float window_height) : 
	graph(graph), graph_edges(sf::PrimitiveType::Lines), window_width(window_width), window_height(window_height) 
{
	// Initialize Quadtree with window bounds
	Quadtree::Bounds graphBounds = { 0, 0, window_width, window_height };  // Adjust bounds
	quadtree = std::make_unique<Quadtree>(graphBounds);

	generateEdges();
}

void Graphics::render(sf::RenderWindow & window) {
	window.draw(graph_edges);
}

void Graphics::changeEdgeColor(long long id, sf::Color new_color) {
	auto [idx1, idx2] = edge_to_vertex[id];

	graph_edges[idx1].color = new_color;
	graph_edges[idx2].color = new_color;
}

const std::pair<sf::Vertex, sf::Vertex>& Graphics::getEdge(long long id) const {
	auto [idx1, idx2] = edge_to_vertex.at(id);
	sf::Vertex vertex1 = graph_edges[idx1];
	sf::Vertex vertex2 = graph_edges[idx2];

	return { vertex1, vertex2 };
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
	const std::unordered_map<long long, Edge>& edges = graph.getEdges();
	graph_edges.resize(edges.size()*2);

	// Iterate over edges and create lines
	// Transform each node to SFML
	std::size_t idx = 0;
	for (const auto& [id, edge] : edges) {
		const Node& from = nodes.at(edge.from);
		const Node& target = nodes.at(edge.to);

		// Map edge id to vertex locations
		edge_to_vertex[id] = { idx, idx + 1 };

		graph_edges[idx].position = transformToSFML(from.lat, from.lon);
		graph_edges[idx++].color = sf::Color::Green;
		graph_edges[idx].position = transformToSFML(target.lat, target.lon);
		graph_edges[idx++].color = sf::Color::Green;
	}
}



