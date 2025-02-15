#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <span>

struct Node {
	long long id;
	double lat; // Latitude
	double lon; // Longitude
};

struct Edge {

	long long from; // Source
	long long to; // Target
};

struct Way {
	long long id;
	std::vector<Edge> edges; 
};

class Graph {

public:
	void addNode(Node node);

	void addWay(Way way);

	void addEdge(Edge edge, Edge reverse_edge);

	// Create graph limits, aka upper left and bottom right corner coordinates
	void addLimits(double max_lat, double min_lat, double max_lon, double min_lon);

	// Create adjacency list
	void createAdj();

	// Get limits indicated by two booleans, is latitude/longitude, is max/min
	double getLimit(bool lat, bool max);

	size_t nodeCount();
	std::span<const Edge> getEdges();

private:
	std::unordered_map<long long, Node> nodes; // ID to node
	std::unordered_map<long long, Way> ways; // ID to way
	std::unordered_map<long long, std::vector<std::pair<long long, double>>> adj_list; // Adjacency list for traversal
	std::vector<Edge> edges;

	double max_lat, min_lon; // Upper left corner
	double min_lat, max_lon; // Lower right corner

	// Helper to get euclidean distance between two nodes
	double getDistance(const Node& from, const Node& to);
};

