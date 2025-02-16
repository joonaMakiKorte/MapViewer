#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

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

private:
	// Dynamically define exact latitude/longitude range
	// which we want to keep nodes from
	struct Bounds {
		double min_lat = 61.48953;
		double max_lat = 61.50902;
		double min_lon = 23.73759;
		double max_lon = 23.78420;

		// For filtering nodes during parsing
		bool contains(double lat, double lon) const {
			return lat >= min_lat && lat <= max_lat && lon >= min_lon && lon <= max_lon;
		}
	};

public:
	Bounds bbox;

	void addNode(Node node);

	void addWay(Way way);

	void addEdge(Edge edge);

	// Create adjacency list
	void createAdj();

	const std::vector<Edge>& getEdges() const;

	const std::unordered_map<long long, Node>& getNodes() const;


private:
	std::unordered_map<long long, Node> nodes; // ID to node
	std::unordered_map<long long, Way> ways; // ID to way
	std::unordered_map<long long, std::vector<std::pair<long long, double>>> adj_list; // Adjacency list for traversal
	std::vector<Edge> edges;

	// Helper to get euclidean distance between two nodes
	double getDistance(const Node& from, const Node& to);
};

