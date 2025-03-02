#include "Algorithm.hpp"

void Algorithm::runAstar(Graph& graph, int64_t source, int64_t target,
	std::vector<uint32_t>& path, std::unordered_set<uint32_t>& path_lookup, double& distance) {
	// Priority queue for A* algorithm
	std::priority_queue<AstarNode> pq;

	// Track visited nodes and their g values
	std::unordered_map<int64_t, double> dist; // g values

	// Track the path and distances
	std::unordered_map<int64_t, std::tuple<int64_t, double, uint32_t>> prev; // (parent, weight, edge_id)

	// Add source node to the priority queue
	AstarNode start(source, 0, heuristic(graph,source,target));
	pq.push(start);
	dist[source] = 0;

	// A* algorithm to find the shortest path from source to target
	while (!pq.empty()) {
		// Get the current node
		AstarNode current = pq.top();
		pq.pop();



		// If target is reached reconstruct the path
		if (current.id == target) {
			for (int64_t at = target; at != source; at = std::get<0>(prev[at])) {
				path.push_back(std::get<2>(prev[at]));
				path_lookup.insert(std::get<2>(prev[at]));
				distance += std::get<1>(prev[at]);
			}
			return;
		}

		// Skip if shorter path is already found
		if (current.g > dist[current.id]) {
			continue;
		}

		// Visit neighbors of the current node
		for (const auto& [neighbor, weight, edge_id] : graph.getNeighbors(current.id)) {
			// Calculate the new distance
			double g = current.g + weight;
			// Update the distance if a shorter path is found
			if (dist.find(neighbor) == dist.end() || g < dist[neighbor]) {
				dist[neighbor] = g; // Update the distance
				AstarNode next(neighbor, g, heuristic(graph,neighbor,target));
				pq.push(next);
				prev[neighbor] = { current.id, weight, edge_id };
			}
		}
	}
	// If no path found distance remains zero
}

double Algorithm::heuristic(Graph& graph, int64_t current, int64_t target) {
	// Get the current and target node
	const Graph::Node& current_node = graph.getNode(current);
	const Graph::Node& target_node = graph.getNode(target);

	// Calculate the heuristic cost using Haversine distance
	return graph.getHaversineDistance(current_node, target_node);
}
