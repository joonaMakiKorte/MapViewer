#pragma once

#include "Graphics.hpp"
#include <queue>

class Algorithm {

public:
	// Run Dijkstra's algorithm to find the shortest path from source to target
	static void runDijkstra(const Graph& graph, int64_t source, int64_t target, std::vector<uint32_t>& path, double& distance);
};

