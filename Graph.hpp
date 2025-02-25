#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <cmath>

constexpr double R = 6371000; // Earth radius in meters
constexpr double PI = 3.14159265358979323846;


class Graph {

public: 
	struct Node {
		double lat; // Latitude
		double lon; // Longitude
	};

	struct Edge {
		int64_t from; // Source
		int64_t to; // Target

		// Equality operator for unordered_set
		bool operator==(const Edge& other) const {
			return from == other.from && to == other.to;
		}
	};

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

	// Custom hash function for Edge
	struct EdgeHash {
		std::size_t operator()(const Edge& edge) const {
			return std::hash<int64_t>()(edge.from) ^ std::hash<int64_t>()(edge.to);
		}
	};

public:
	Bounds bbox;

	void addNode(int64_t id, Node node);

	bool addEdge(uint32_t id, Edge edge);

	bool hasNode(int64_t id);

	bool hasEdge(int64_t from, int64_t to) const;

	// Create adjacency list
	void createAdj();

	const std::unordered_map<uint32_t, Edge>& getEdges() const;

	const std::unordered_map<int64_t, Node>& getNodes() const;

	// Get endpoint nodes of edge by id
	std::pair<const Node&, const Node&> getEdgeNodes(uint32_t edge_id) const;

	// Get neighbors of a node by id
	const std::vector<std::tuple<int64_t, double, uint32_t>>& getNeighbors(int64_t id) const;

private:
	// Calculate the distance between two nodes using Haversine formula
	double getHaversineDistance(const Node& from, const Node& to);

	// Helper function to convert degrees to radians
	double toRadians(double degrees);

private:
	std::unordered_map<int64_t, Node> nodes; // ID to node
	std::unordered_map<uint32_t, Edge> edges; // ID to edge
	std::unordered_set<Edge, EdgeHash> edge_set; // For fast edge lookup
	std::unordered_map<int64_t, std::vector<std::tuple<int64_t, double, uint32_t>>> adj_list; // <neighbor_id, weight, edge_id>
};

