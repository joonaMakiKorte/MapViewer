#pragma once

#include "Graphics.hpp"
#include <queue>

class Algorithm {

public:
	// Run Dijkstra's algorithm to find the shortest path from source to target
	// If a path is found, store the edge IDs in the path vector and the total distance (meters) in the distance reference
	static void runDijkstra(const Graph& graph, int64_t source, int64_t target, std::vector<uint32_t>& path, double& distance);
};

