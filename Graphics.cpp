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

	// Set the selection circle properties
	selection_circle.setFillColor(sf::Color::Transparent);
	selection_circle.setOutlineColor(sf::Color::Red);
	selection_circle.setOutlineThickness(2.0f);
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

	// Draw the selection circle
	if (selection_circle.getOutlineColor() != sf::Color::Transparent) {
		window.draw(selection_circle);
	}
}

void Graphics::changeEdgeColor(long long id, sf::Color new_color) {
	// Find the edge by ID
	auto it = graph_edges.find(id);
	if (it == graph_edges.end()) {
		std::cerr << "Edge with ID " << id << " not found!" << std::endl;
		return;
	}
	// Change the color of the edge
	it->second->v1.color = new_color;
	it->second->v2.color = new_color;
}

void Graphics::rescaleGraphics(float new_width, float new_height) {
	// Update window size
	window_width = new_width;
	window_height = new_height;

	// Initialize new Quadtree with new window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height };
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Update nodes graphics coordinates with new window size
	for (auto& [id, edge_ptr] : graph_edges) {
		auto& edge = *edge_ptr;  // Dereference the pointer to get the actual edge

		// Get nodes by edge id
		auto [from, target] = graph.getNodes(id);

		sf::Vector2f pos1 = transformToSFML(from.lat, from.lon);
		sf::Vector2f pos2 = transformToSFML(target.lat, target.lon);

		edge.v1.position = pos1;
		edge.v2.position = pos2;

		// Insert into the new Quadtree
		quadtree->insert(&edge);
	};
}

void Graphics::selectNode(sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mouse_pos) {
	// Convert pixel coordinates to world coordinates
	sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos, view);

	// Define a small bounding box around the click position
	float radius = 5.0f;
	Quadtree::Bounds query_bounds = { world_pos.x - radius, world_pos.y - radius, world_pos.x + radius, world_pos.y + radius };

	// Query the quadtree for edges within the bounding box
	std::vector<TreeEdge*> result;
	quadtree->query(query_bounds, result);

	// Check if any edges were within the bounding box
	if (result.empty()) {
		std::cout << "No edges found near the click position" << std::endl;
		return;
	}

	// Get the closest node to the click position
	sf::Vertex* closest_node = getClosestNode(world_pos, result);
	if (closest_node) {
		// Draw a circle around the selected node
		selection_circle.setRadius(radius);
		selection_circle.setOrigin({radius,radius});
		selection_circle.setPosition(closest_node->position);
	}
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

sf::Vertex* Graphics::getClosestNode(const sf::Vector2f& world_pos, const std::vector<TreeEdge*>& edges) {
	sf::Vertex* closest_node = nullptr;
	float min_distance = std::numeric_limits<float>::max();
	// Iterate over all edges and find the closest node
	for (auto* edge : edges) {
		float dist1 = distance(world_pos, edge->v1.position);
		float dist2 = distance(world_pos, edge->v2.position);
		if (dist1 < min_distance) {
			min_distance = dist1;
			closest_node = &edge->v1;
		}
		if (dist2 < min_distance) {
			min_distance = dist2;
			closest_node = &edge->v2;
		}
	}
	return closest_node;
}

float Graphics::distance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	return std::sqrt(dx * dx + dy * dy);
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of edges * 2
	const std::unordered_map<long long, Edge>& edges = graph.getEdges();
	graph_edges.reserve(edges.size()*2);

	// Iterate over edges and create vertexes
	// Transform each node to SFML
	for (const auto& [id, edge] : edges) {
		auto [from, target] = graph.getNodes(id);

		// Create vertexes
		sf::Vertex v1, v2;
		v1.position = transformToSFML(from.lat, from.lon);
		v1.color = sf::Color::Green;
		v2.position = transformToSFML(target.lat, target.lon);
		v2.color = sf::Color::Green;

		// Create a new TreeEdge pointer
		auto tree_edge = std::make_unique<TreeEdge>();
		tree_edge->v1 = v1;
		tree_edge->v2 = v2;

		// Insert to datastructure and quadtree
		TreeEdge* edge_ptr = tree_edge.get();
		quadtree->insert(edge_ptr);
		graph_edges[id] = std::move(tree_edge);
	}
}



