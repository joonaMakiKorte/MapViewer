#include "Graphics.hpp"
#include "Quadtree.hpp"

Graphics::Graphics(Graph& graph, float window_width, float window_height) : 
	graph(graph), visible_edges(sf::PrimitiveType::Lines), window_width(window_width), window_height(window_height) 
{
	// Initialize Quadtree with window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height }; 
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Generate edges and insert to quadtree
	generateEdges();
}

void Graphics::render(sf::RenderWindow& window, const sf::View& view) {
	visible_edges.clear(); // Clear previous frame data

	// Get the bounding box of the current view
	Quadtree::Bounds view_bounds = getViewBounds(view);

	// Get the visible edges from the quadtree
	std::vector<TreeEdge*> new_visible_edges;
	quadtree->query(view_bounds, new_visible_edges);

	// Store new visible edges in the VertexArray for efficient rendering
	for (auto* edge : new_visible_edges) {
		visible_edges.append(edge->v1);
		visible_edges.append(edge->v2);
	}

	window.draw(visible_edges);
}

void Graphics::rescaleGraphics(float new_width, float new_height) {
	// Update window size
	window_width = new_width;
	window_height = new_height;

	// Initialize new Quadtree with new window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height };
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Update nodes graphics coordinates with new window size
	for (auto& edge_ptr : graph_edges) {
		auto& edge = *edge_ptr;  // Dereference the pointer to get the actual edge

		sf::Vector2f pos1 = transformToSFML(edge.lat1, edge.lon1);
		sf::Vector2f pos2 = transformToSFML(edge.lat2, edge.lon2);

		edge.v1.position = pos1;
		edge.v2.position = pos2;

		// Insert into the new Quadtree
		quadtree->insert(edge.id, &edge);
	};
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

Quadtree::Bounds Graphics::getViewBounds(const sf::View& view) {
	sf::Vector2f center = view.getCenter();
	sf::Vector2f size = view.getSize();  // The size of the view (in world units)

	return {
		center.x - size.x / 2.0f,  // Left
		center.y - size.y / 2.0f,  // Top
		center.x + size.x / 2.0f,  // Right
		center.y + size.y / 2.0f   // Bottom
	};
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of edges * 2
	const std::unordered_map<long long, Node>& nodes = graph.getNodes();
	const std::unordered_map<long long, Edge>& edges = graph.getEdges();
	graph_edges.reserve(edges.size()*2);

	// Iterate over edges and create vertexes
	// Transform each node to SFML
	for (const auto& [id, edge] : edges) {
		const Node& from = nodes.at(edge.from);
		const Node& target = nodes.at(edge.to);

		// Create vertexes
		sf::Vertex v1, v2;
		v1.position = transformToSFML(from.lat, from.lon);
		v1.color = sf::Color::Green;
		v2.position = transformToSFML(target.lat, target.lon);
		v2.color = sf::Color::Green;

		// Create a new TreeEdge pointer
		auto tree_edge = std::make_unique<TreeEdge>();
		tree_edge->id = id;

		tree_edge->lat1 = from.lat;
		tree_edge->lon1 = from.lon;
		tree_edge->lat2 = target.lat;
		tree_edge->lon2 = target.lon;

		tree_edge->v1 = v1;
		tree_edge->v2 = v2;

		// Insert to datastructure and quadtree
		TreeEdge* edge_ptr = tree_edge.get();
		quadtree->insert(id, edge_ptr);
		graph_edges.push_back(std::move(tree_edge));
	}
}



