#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <iostream>

struct Node {
	double lat; // Latitude
	double lon; // Longitude
};

struct Edge {
	int64_t from; // Source
	int64_t to; // Target
};

struct Way {
	std::vector<uint32_t> edges; 
};

class Graph {

private:
	// Exact latitude/longitude range which we want to keep nodes from
	// Aka bounding box
	struct Bounds {
		double min_lat;
		double max_lat;
		double min_lon;
		double max_lon ;

		// For filtering nodes during parsing
		bool contains(double lat, double lon) const {
			return lat >= min_lat && lat <= max_lat && lon >= min_lon && lon <= max_lon;
		}
	};

public:
	Bounds bbox;

	void addNode(int64_t id, Node node);

	void addWay(int64_t id, Way way);

	void addEdge(uint32_t id, Edge edge);

	bool hasNode(int64_t id);

	// Create adjacency list
	void createAdj();

	const std::unordered_map<uint32_t, Edge>& getEdges() const;

	// Get nodes by edge id
	std::pair<const Node&, const Node&> getNodes(uint32_t edge_id) const;

private:
	std::unordered_map<int64_t, Node> nodes; // ID to node
	std::unordered_map<uint32_t, Edge> edges; // ID to edge
	std::unordered_map<int64_t, Way> ways; // ID to way
	std::unordered_map<int64_t, std::vector<std::tuple<uint64_t, double, uint32_t>>> adj_list; // <neighbor_id, weight, edge_id>
	
	// Helper to get euclidean distance between two nodes
	double getDistance(const Node& from, const Node& to);
};

