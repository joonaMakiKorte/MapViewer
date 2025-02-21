#include "Graph.hpp"

void Graph::addNode(int64_t id, Node node) {
	nodes[id] = node;
}

void Graph::addWay(int64_t id, Way way) {
	ways[id] = way;
}

void Graph::addEdge(uint32_t id, Edge edge) {
	edges[id] = edge;
}

bool Graph::hasNode(int64_t id) {
	return nodes.find(id) != nodes.end();
}

void Graph::createAdj() {
	// Iterate over edges
	for (const auto& [id, edge] : edges) {
		// Get endpoint nodes and calculate weight
		const Node& from = nodes[edge.from];
		const Node& to = nodes[edge.to];
		double weight = getDistance(from, to);

		// Create adj list entry
		adj_list[edge.from].emplace_back(edge.to, weight, id);
		adj_list[edge.to].emplace_back(edge.from, weight, id);
	}
}

const std::unordered_map<uint32_t, Edge>& Graph::getEdges() const {
	return edges;
}

std::pair<const Node&, const Node&> Graph::getNodes(uint32_t edge_id) const {
	// Get edge by id
	auto it = edges.find(edge_id);
	if (it == edges.end()) {
		throw std::runtime_error("Edge not found");
	}

	// Get source and target nodes
	int64_t from_id = it->second.from;
	int64_t target_id = it->second.to;

	auto from_it = nodes.find(from_id);
	auto target_it = nodes.find(target_id);
	if (from_it == nodes.end() || target_it == nodes.end()) {
		throw std::runtime_error("Node not found");
	}

	return { from_it->second, target_it->second };
}

double Graph::getDistance(const Node& from, const Node& to) {
	double dx = to.lon - from.lon;
	double dy = to.lat - from.lat;
	return sqrt(dx * dx + dy * dy);
}
