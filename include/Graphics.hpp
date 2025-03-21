#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Quadtree.hpp"
#include "Graph.hpp"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

constexpr int64_t UNASSIGNED = -1; // Sentinel value for unassigned node ID

// Colors for map
constexpr sf::Color MAP_COLOR = sf::Color(0, 255, 255); // Neon cyan
constexpr sf::Color PATH_COLOR = sf::Color(255, 0, 255); // Magenta

// Edge thickness for map
constexpr float MAP_THICKNESS = 1.0f;
constexpr float PATH_THICKNESS = 3.0f;

// Click radius for node selection
constexpr float CLICK_RADIUS = 2.5f;

class Graphics {
public:
	// Constructor
	Graphics(Graph& graph, float window_width, float window_height);

	// Render map, aka display graph edges
	void render(sf::RenderWindow& window, const sf::View& view);

	// When window gets resized, rescale nodes
	void rescaleGraphics(float new_width, float new_height);

	// Handle selecting a node by mouse click
	void selectNode(sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mouse_pos);

	// Handle finding a route between two nodes, return boolean indicating if can be executed
	// If so, call Algorithm::runAstar to find the shortest path and highlight path
	// If no path is found, distance reference remains zero 
	void findRoute();

private:
	// Generate graph edges and insert to quadtree
	void generateEdges();

	// Initialize window elements:
	// Selection circles and text box
	void initWindowElements();

	// Transform given coordinates of latitude and longitude to graphics coordinates
	sf::Vector2f transformToSFML(double lat, double lon);

	// Change edge color and set thickness by ID
	void changeEdgeColor(uint32_t id, sf::Color new_color, float new_thickness);

	// Get the view bounds for current view as Bounds-struct
	Quadtree::Bounds getViewBounds(const sf::View& view);

	// Get the node closest to the given world position
	// Returns a pointer to the closest node and modifies reference id, nullptr if not found
	// Takes a vector of TreeEdge-pointers to search from
	sf::Vector2f* getClosestNode(const sf::Vector2f& world_pos, const std::vector<Quadtree::TreeEdge*>& edges, int64_t& selected_id);

	// Calculate the Euclidean distance between two points
	float distance(const sf::Vector2f& p1, const sf::Vector2f& p2);

	// Highlight the edges of a path given as a vector
	void highlightPath(const std::vector<uint32_t>& path, sf::Color new_color, float new_thickness);

	// Form thick lines to represent graph edges by rendering each edge as two triangles that form a rectangle
	// Takes the edges to render as the parameter
	// Also takes the vertex arrays to render as references, triangles get added to either depending on if the edge is in current found path
	void renderEdges(std::vector<Quadtree::TreeEdge*> new_visible_edges, sf::VertexArray& rendered_edges, sf::VertexArray& rendered_path);

private:
	Graph& graph;
	std::unique_ptr<Quadtree> quadtree;

	// Store all the graph edges as TreeEdge-structs that get used in Quadtree
	// Access by ID
	std::unordered_map<uint32_t, std::unique_ptr<Quadtree::TreeEdge>> graph_edges;

	// Keep track of current window resolution
	float window_width;
	float window_height;

	// Circle shape for node selection
	sf::CircleShape from_circle;
	sf::CircleShape target_circle;

	// Track selected nodes by id
	int64_t from_id;
	int64_t target_id;

	std::vector<uint32_t> found_path; // Track the found path
	std::unordered_set<uint32_t> found_path_lookup; // For fast lookup

	// Mutex for thread safety
	std::mutex graphics_mutex;
};

#endif