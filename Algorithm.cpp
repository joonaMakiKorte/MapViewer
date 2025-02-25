#include "Algorithm.hpp"

void Algorithm::runDijkstra(const Graph& graph, int64_t source, int64_t target, std::vector<uint32_t>& path, double& distance) {
	// Initialize distances and visited arrays
	std::unordered_map<int64_t, double> dist;
	std::unordered_map<int64_t, std::tuple<int64_t, double, uint32_t>> prev; // (parent, weight, edge_id)

	// Initialize min-heap priority queue
	using pqElement = std::tuple<double, int64_t>;
	std::priority_queue<pqElement, std::vector<pqElement>, std::greater<>> pq;

	// Initialize distances
	for (const auto& [id, node] : graph.getNodes()) {
		dist[id] = std::numeric_limits<double>::max();
	}

	// Visit the source node
	dist[source] = 0;
	pq.push({ 0, source});
	prev[source] = { UNASSIGNED, 0, UNASSIGNED }; // Starting node has no parent

	// Dijkstra's algorithm to find the shortest path from source to target
	while (!pq.empty()) {
		auto [current_distance, current] = pq.top();
		pq.pop();

		// Early exit if target is reached
		if (current == target) break;

		// Visit neighbors of the current node
		for (const auto& [neighbor, weight, edge_id] : graph.getNeighbors(current)) {
			// Calculate the new distance
			double alt = current_distance + weight;
			// Update the distance if a shorter path is found
			if (alt < dist[neighbor]) {
				dist[neighbor] = alt;
				prev[neighbor] = { current, weight, edge_id };
				pq.push({ alt, neighbor});
			}
		}
	}

	// No path exists if target distance is still infinity
	if (dist[target] == std::numeric_limits<double>::max()) {
		return;
	}

	// Reconstruct the path from source to target
	for (int64_t at = target; at != UNASSIGNED; at = std::get<0>(prev[at])) {
		path.push_back(std::get<2>(prev[at]));
		distance += std::get<1>(prev[at]);
	}
	path.pop_back(); // Remove the last edge as it is the source node
}
