#pragma once

#include "Graphics.hpp"
#include <queue>

class Algorithm {

private: 
	// Define a struct for A* node
	struct AstarNode {
		int64_t id; // Node ID
		double g, h; // Cost from start, heuristic cost to target

		AstarNode(int64_t id, double g, double h) : id(id), g(g), h(h) {}

		// Calculate the total cost f = g + h
		double f() const {
			return g + h;
		}

		// Overload the comparison operator for the priority queue
		bool operator<(const AstarNode& other) const {
			return f() > other.f(); // Min-heap
		}
	};

public:
	// Run Dijkstra's algorithm to find the shortest path from source to target
	// Visualize the path finding by highlighting the edges visited
	// If a path is found, store the edge IDs in the path vector and the total distance (meters) in the distance reference
	static void runDijkstra(const Graph& graph, int64_t source, int64_t target, std::vector<uint32_t>& path, double& distance);

	static void runAstar(Graph& graph, int64_t source, int64_t target, std::vector<uint32_t>& path, double& distance);

private:
	// Helper function for A* to calculate the heuristic cost from current node to target node 
	static double heuristic(Graph& graph, int64_t current, int64_t target);
};

