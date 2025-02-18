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
	std::vector<long long> edges; 
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

	void addNode(Node node);

	void addWay(Way way);

	void addEdge(long long id, Edge edge);

	bool hasNode(long long id);

	// Create adjacency list
	void createAdj();

	const std::unordered_map<long long, Edge>& getEdges() const;

	const std::unordered_map<long long, Node>& getNodes() const;


private:
	std::unordered_map<long long, Node> nodes; // ID to node
	std::unordered_map<long long, Edge> edges; // ID to edge
	std::unordered_map<long long, Way> ways; // ID to way
	std::unordered_map<long long, std::vector<std::tuple<long long, double, long long>>> adj_list; // <neighbor_id, weight, edge_id>
	
	// Helper to get euclidean distance between two nodes
	double getDistance(const Node& from, const Node& to);
};

