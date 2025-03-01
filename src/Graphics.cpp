#include "Graphics.hpp"
#include "Algorithm.hpp"
#include <iostream>
#include <future>

Graphics::Graphics(Graph& graph, float window_width, float window_height) : 
	graph(graph), visible_edges(sf::PrimitiveType::Triangles), 
	window_width(window_width), window_height(window_height),
	from_id(UNASSIGNED), target_id(UNASSIGNED)
{
	// Initialize Quadtree with window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height }; 
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Generate edges and insert to quadtree
	generateEdges();

	// Initialize selection circles
	from_circle.setRadius(5.0f);
	target_circle.setRadius(5.0f);
	from_circle.setFillColor(sf::Color::Red);
	target_circle.setFillColor(sf::Color::Red);
}

void Graphics::render(sf::RenderWindow& window, const sf::View& view) {
	visible_edges.clear(); // Clear previous frame data

	// Get the bounding box of the current view
	Quadtree::Bounds view_bounds = getViewBounds(view);

	// Get the visible edges from the quadtree
	std::vector<Quadtree::TreeEdge*> new_visible_edges;
	quadtree->query(view_bounds, new_visible_edges);

	// Update visible edges to queried and calculate triangles (lines) to render
	renderEdges(new_visible_edges, 1.0f);

	window.draw(visible_edges);

	// Draw the selection circles
	if (from_id != UNASSIGNED) {
		window.draw(from_circle);
	}
	if (target_id != UNASSIGNED) {
		window.draw(target_circle);
	}
}

void Graphics::changeEdgeColor(uint32_t id, sf::Color new_color) {
	std::lock_guard<std::mutex> lock(graphics_mutex);

	// Find the edge by ID
	auto it = graph_edges.find(id);
	if (it == graph_edges.end()) {
		std::cerr << "Edge with ID " << id << " not found!" << std::endl;
		return;
	}
	// Change the color of the edge
	it->second->color = new_color;
}

void Graphics::rescaleGraphics(float new_width, float new_height) {
	// Update window size
	window_width = new_width;
	window_height = new_height;

	// Hide selection circles since they are not valid anymore
	from_id = UNASSIGNED;
	target_id = UNASSIGNED;

	// Initialize new Quadtree with new window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height };
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Update nodes graphics coordinates with new window size
	for (auto& [id, edge_ptr] : graph_edges) {
		auto& edge = *edge_ptr;  // Dereference the pointer to get the actual edge

		// Get nodes by edge id
		auto [from, target] = graph.getEdgeNodes(id);

		sf::Vector2f pos1 = transformToSFML(from.lat, from.lon);
		sf::Vector2f pos2 = transformToSFML(target.lat, target.lon);

		edge.v1 = pos1;
		edge.v2 = pos2;

		// Insert into the new Quadtree
		quadtree->insert(&edge);
	};
}

void Graphics::selectNode(sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mouse_pos) {
	// Convert pixel coordinates to world coordinates
	sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos, view);

	// Define a small bounding box around the click position
	float radius = 2.5f;
	Quadtree::Bounds query_bounds = { world_pos.x - radius, world_pos.y - radius, world_pos.x + radius, world_pos.y + radius };

	// Query the quadtree for edges within the bounding box
	std::vector<Quadtree::TreeEdge*> result;
	quadtree->query(query_bounds, result);

	// Check if any edges were within the bounding box
	if (result.empty()) { 
		return;
	}

	// Get the closest node to the click position
	int64_t selected_id = UNASSIGNED;
	sf::Vector2f* closest_node = getClosestNode(world_pos, result, selected_id);
	if (closest_node) {
		// Check if we have already selected the same node
		// If so, deselect it
		if (from_id == selected_id) {
			from_id = UNASSIGNED;
		}
		else if (target_id == selected_id) {
			target_id = UNASSIGNED;
		}
		else {
			// Select the node depending on which one is already selected
			// If both are selected, update the target node
			if (from_id == UNASSIGNED) {
				from_id = selected_id;
				from_circle.setOrigin({ radius,radius });
				from_circle.setPosition(*closest_node);
			}
			else {
				target_id = selected_id;
				target_circle.setOrigin({ radius,radius });
				target_circle.setPosition(*closest_node);
			}
		}
	}
}

void Graphics::findRoute() {
	if (from_id == UNASSIGNED || target_id == UNASSIGNED) {
		std::cerr << "Both from and target nodes must be selected!" << std::endl;
		return;
	}

	highlightPath(current_path, MAP_COLOR); // Reset edge colors of previous path
	current_path.clear(); // Clear previous path

	// Run A* algorithm in a separate thread
	// This is done to prevent the GUI from freezing
	double distance = 0;
	std::future<void> future = std::async(std::launch::async, [&]() {
		Algorithm::runAstar(graph, from_id, target_id, current_path, distance);
	});
	future.wait(); // Wait for the A* algorithm to finish

	if (current_path.empty()) {
		std::cout << "No route found!" << std::endl;
		return;
	}
	// Highlight the found path
	highlightPath(current_path, PATH_COLOR);

	if (distance < 1000) {
		// Display full meters if distance is less than a kilometer
		std::cout << "Distance: " << static_cast<int>(distance) << "m" << std::endl;
	}
	else {
		// Display kilometers with one decimal place
		std::cout << "Distance: " << std::fixed << std::setprecision(1) << distance / 1000 << "km" << std::endl;
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

sf::Vector2f* Graphics::getClosestNode(const sf::Vector2f& world_pos, const std::vector<Quadtree::TreeEdge*>& edges, int64_t& selected_id) {
	sf::Vector2f* closest_node = nullptr;
	float min_distance = std::numeric_limits<float>::max();
	// Iterate over all edges and find the closest node
	for (auto* edge : edges) {
		float dist1 = distance(world_pos, edge->v1);
		float dist2 = distance(world_pos, edge->v2);
		if (dist1 < min_distance) {
			min_distance = dist1;
			closest_node = &edge->v1;
			selected_id = edge->v1_id;
		}
		if (dist2 < min_distance) {
			min_distance = dist2;
			closest_node = &edge->v2;
			selected_id = edge->v2_id;
		}
	}
	return closest_node;
}

float Graphics::distance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	return std::sqrt(dx * dx + dy * dy);
}

void Graphics::highlightPath(const std::vector<uint32_t>& path, sf::Color new_color) {
	// Loop over every edge id in current path and change color
	for (uint32_t id : path) {
		changeEdgeColor(id, new_color);
	}
}

void Graphics::renderEdges(std::vector<Quadtree::TreeEdge*> new_visible_edges, float thickness) {
	// Go over visible edges and create two triangles per edge to add to the vertexarray to render
	for (const auto& edge : new_visible_edges) {
		const sf::Vector2f& start = edge->v1;
		const sf::Vector2f& end = edge->v2;

		sf::Vector2f direction = end - start; // Calculate direction of line
		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Calculate the length (sqrt(x^2+y^2))
		sf::Vector2f unit_dir = direction / length; // Get the unit vector
		sf::Vector2f normal(-unit_dir.y, unit_dir.x); // Calculate the normal of the line

		// Calculate the offset vector
		// Offset vector will be the side of the triangle perpendicular to the line rendered
		// Has length of 1/2 of the desired line thickness since line will be expanded to both sides
		sf::Vector2f offset = normal * (thickness * 0.5f);

		// Create the two triangles
		visible_edges.append(sf::Vertex(start - offset, edge->color));
		visible_edges.append(sf::Vertex(start + offset, edge->color));
		visible_edges.append(sf::Vertex(end + offset, edge->color));

		visible_edges.append(sf::Vertex(start - offset, edge->color));
		visible_edges.append(sf::Vertex(end + offset, edge->color));
		visible_edges.append(sf::Vertex(end - offset, edge->color));
	}
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of edges * 2
	const std::unordered_map<uint32_t, Graph::Edge>& edges = graph.getEdges();
	graph_edges.reserve(edges.size()*2);

	// Iterate over edges and create vertexes
	// Transform each node to SFML
	for (const auto& [id, edge] : edges) {
		auto [from, target] = graph.getEdgeNodes(id);

		// Calculate sfml coordinates of edge endpoint nodes
		sf::Vector2f v1, v2;
		v1 = transformToSFML(from.lat, from.lon);
		v2 = transformToSFML(target.lat, target.lon);

		// Create a new TreeEdge pointer
		auto tree_edge = std::make_unique<Quadtree::TreeEdge>();
		tree_edge->v1 = v1;
		tree_edge->v2 = v2;
		tree_edge->v1_id = edge.from;
		tree_edge->v2_id = edge.to;
		tree_edge->color = MAP_COLOR;

		// Insert to datastructure and quadtree
		Quadtree::TreeEdge* edge_ptr = tree_edge.get();
		quadtree->insert(edge_ptr);
		graph_edges[id] = std::move(tree_edge);
	}
}



