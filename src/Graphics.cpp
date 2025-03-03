#include "Graphics.hpp"
#include "Algorithm.hpp"
#include <iostream>
#include <future>

Graphics::Graphics(Graph& graph, float window_width, float window_height) : 
	graph(graph), window_width(window_width), window_height(window_height),
	from_id(UNASSIGNED), target_id(UNASSIGNED)
{
	// Initialize Quadtree with window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height }; 
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Generate edges and insert to quadtree
	generateEdges();

	// Initialize window elements
	initWindowElements();
}

void Graphics::generateEdges() {
	// Preallocate space
	// Expected vertex count is amount of edges * 2
	const std::unordered_map<uint32_t, Graph::Edge>& edges = graph.getEdges();
	graph_edges.reserve(edges.size() * 2);

	// Iterate over edges and create vertexes
	// Transform each node to SFML
	for (const auto& [id, edge] : edges) {
		// Get nodes
		const Graph::Node& from = graph.getNode(edge.from);
		const Graph::Node& target = graph.getNode(edge.to);

		// Create a new TreeEdge pointer
		auto tree_edge = std::make_unique<Quadtree::TreeEdge>();
		tree_edge->id = id;
		// Calculate sfml coordinates of edge endpoint nodes
		tree_edge->v1 = transformToSFML(from.lat, from.lon);
		tree_edge->v2 = transformToSFML(target.lat, target.lon);
		tree_edge->color = MAP_COLOR; // Initialize to default map color
		tree_edge->thickness = MAP_THICKNESS; // Standard thickness

		// Insert to datastructure and quadtree
		Quadtree::TreeEdge* edge_ptr = tree_edge.get();
		quadtree->insert(edge_ptr);
		graph_edges[id] = std::move(tree_edge);
	}
}

void Graphics::initWindowElements() {
	// Initialize selection circles
	from_circle.setRadius(5.0f);
	from_circle.setOrigin({ CLICK_RADIUS,CLICK_RADIUS });
	from_circle.setOutlineThickness(2.0f);
	from_circle.setFillColor(PATH_COLOR);
	from_circle.setOutlineColor(sf::Color::White);

	target_circle.setRadius(5.0f);
	target_circle.setOrigin({ CLICK_RADIUS,CLICK_RADIUS });
	target_circle.setOutlineThickness(2.0f);
	target_circle.setFillColor(PATH_COLOR);
	target_circle.setOutlineColor(sf::Color::White);
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

void Graphics::changeEdgeColor(uint32_t id, sf::Color new_color, float new_thickness) {
	std::lock_guard<std::mutex> lock(graphics_mutex);

	// Find the edge by ID
	auto it = graph_edges.find(id);
	if (it == graph_edges.end()) {
		std::cerr << "Edge with ID " << id << " not found!" << std::endl;
		return;
	}
	// Change the color and thickness
	it->second->color = new_color;
	it->second->thickness = new_thickness;
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
			selected_id = graph.getEdge(edge->id).from;
			// Check if selected id is same as already selected node (from or target). If so, return
			if (selected_id == from_id || selected_id == target_id) break;
		}
		if (dist2 < min_distance) {
			min_distance = dist2;
			closest_node = &edge->v2;
			selected_id = graph.getEdge(edge->id).to;

			if (selected_id == from_id || selected_id == target_id) break;
		}
	}
	return closest_node;
}

float Graphics::distance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	return std::sqrt(dx * dx + dy * dy);
}

void Graphics::highlightPath(const std::vector<uint32_t>& path, sf::Color new_color, float new_thickness) {
	// Loop over every edge id in current path and change color
	for (uint32_t id : path) {
		changeEdgeColor(id, new_color, new_thickness);
	}
}

void Graphics::renderEdges(std::vector<Quadtree::TreeEdge*> new_visible_edges, sf::VertexArray& rendered_edges, sf::VertexArray& rendered_path) {
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
		sf::Vector2f offset = normal * (edge->thickness * 0.5f);

		// Create the two triangles
		sf::Vertex triangle1_a(start - offset, edge->color);
		sf::Vertex triangle1_b(start + offset, edge->color);
		sf::Vertex triangle1_c(end + offset, edge->color);

		sf::Vertex triangle2_a(start - offset, edge->color);
		sf::Vertex triangle2_b(end + offset, edge->color);
		sf::Vertex triangle2_c(end - offset, edge->color);

		// Append to either VertexArray depending on if edge is on path
		if (found_path_lookup.contains(edge->id)) {
			rendered_path.append(triangle1_a);
			rendered_path.append(triangle1_b);
			rendered_path.append(triangle1_c);
			rendered_path.append(triangle2_a);
			rendered_path.append(triangle2_b);
			rendered_path.append(triangle2_c);
		}
		else {
			rendered_edges.append(triangle1_a);
			rendered_edges.append(triangle1_b);
			rendered_edges.append(triangle1_c);
			rendered_edges.append(triangle2_a);
			rendered_edges.append(triangle2_b);
			rendered_edges.append(triangle2_c);
		}
	}
}

void Graphics::render(sf::RenderWindow& window, const sf::View& view) {
	// Initialize new vertex arrays to render
	// Two are initialized, since the path is drawn last so that it doesn't get covered by any overlapping edges
	sf::VertexArray rendered_edges{ sf::PrimitiveType::Triangles }; // Contains visible edges excluding path
	sf::VertexArray rendered_path{ sf::PrimitiveType::Triangles }; // Contains path

	// Get the bounding box of the current view
	Quadtree::Bounds view_bounds = getViewBounds(view);

	// Get the visible edges from the quadtree
	std::vector<Quadtree::TreeEdge*> new_visible_edges;
	quadtree->query(view_bounds, new_visible_edges);

	// Update visible edges to queried and calculate triangles (lines) to render
	renderEdges(new_visible_edges, rendered_edges, rendered_path);

	// Draw edges
	window.draw(rendered_edges);
	window.draw(rendered_path);

	// Draw the selection circles
	if (from_id != UNASSIGNED) {
		window.draw(from_circle);
	}
	if (target_id != UNASSIGNED) {
		window.draw(target_circle);
	}
}

void Graphics::rescaleGraphics(float new_width, float new_height) {
	// Update window size
	window_width = new_width;
	window_height = new_height;

	// Calculate new coordinates for selection circles
	if (from_id != UNASSIGNED) {
		const Graph::Node& from_node = graph.getNode(from_id);
		sf::Vector2f from_pos = transformToSFML(from_node.lat, from_node.lon);
		from_circle.setOrigin({ CLICK_RADIUS,CLICK_RADIUS });
		from_circle.setPosition({ from_pos });
	}
	if (target_id != UNASSIGNED) {
		const Graph::Node& target_node = graph.getNode(target_id);
		sf::Vector2f target_pos = transformToSFML(target_node.lat, target_node.lon);
		target_circle.setOrigin({ CLICK_RADIUS,CLICK_RADIUS });
		target_circle.setPosition({ target_pos });
	}

	// Initialize new Quadtree with new window bounds
	Quadtree::Bounds graph_bounds = { 0, 0, window_width, window_height };
	quadtree = std::make_unique<Quadtree>(graph_bounds);

	// Update nodes graphics coordinates with new window size
	for (auto& [id, edge_ptr] : graph_edges) {
		auto& edge = *edge_ptr;  // Dereference the pointer to get the actual edge

		// Get nodes by edge id
		const Graph::Edge& graph_edge = graph.getEdge(id);
		const Graph::Node& from = graph.getNode(graph_edge.from);
		const Graph::Node& target = graph.getNode(graph_edge.to);

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
	Quadtree::Bounds query_bounds = { world_pos.x - CLICK_RADIUS, world_pos.y - CLICK_RADIUS, world_pos.x + CLICK_RADIUS, world_pos.y + CLICK_RADIUS };

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
				from_circle.setPosition(*closest_node);
			}
			else {
				target_id = selected_id;
				
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

	highlightPath(found_path, MAP_COLOR, MAP_THICKNESS); // Reset edge colors and thickness of previous path
	found_path.clear(); // Clear previous path
	found_path_lookup.clear();

	// Run A* algorithm in a separate thread
	// This is done to prevent the GUI from freezing
	double distance = 0;
	std::future<void> future = std::async(std::launch::async, [&]() {
		Algorithm::runAstar(graph, from_id, target_id, found_path, found_path_lookup, distance);
	});
	future.wait(); // Wait for the A* algorithm to finish

	if (found_path.empty()) {
		std::cout << "No route found!" << std::endl;
		return;
	}
	// Highlight the found path
	highlightPath(found_path, PATH_COLOR, PATH_THICKNESS);
	
	if (distance < 1000) {
		// Display full meters if distance is less than a kilometer
		std::cout << "Distance: " << static_cast<int>(distance) << "m" << std::endl;
	}
	else {
		// Display kilometers with one decimal place
		std::cout << "Distance: " << std::fixed << std::setprecision(1) << distance / 1000 << "km" << std::endl;
	}
}