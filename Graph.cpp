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
		double weight = getHaversineDistance(from, to);

		// Create adj list entry
		adj_list[edge.from].emplace_back(edge.to, weight, id);
		adj_list[edge.to].emplace_back(edge.from, weight, id);
	}
}

const std::unordered_map<uint32_t, Edge>& Graph::getEdges() const {
	return edges;
}

const std::unordered_map<int64_t, Node>& Graph::getNodes() const {
	return nodes;
}

std::pair<const Node&, const Node&> Graph::getEdgeNodes(uint32_t edge_id) const {
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

const std::vector<std::tuple<int64_t, double, uint32_t>>& Graph::getNeighbors(int64_t id) const {
	return adj_list.at(id);
}

double Graph::getHaversineDistance(const Node& from, const Node& to) {
	double phi1 = toRadians(from.lat);
	double phi2 = toRadians(to.lat);
	double delta_phi = toRadians(to.lat - from.lat);
	double delta_lambda = toRadians(to.lon - from.lon);

	// a = sin^2(delta_phi / 2) + cos(phi1) * cos(phi2) * sin^2(delta_lambda / 2)
	double a = std::sin(delta_phi / 2.0) * std::sin(delta_phi / 2.0) +
		std::cos(phi1) * std::cos(phi2) * 
		std::sin(delta_lambda / 2.0) * std::sin(delta_lambda / 2.0);
	
	// c = 2 * atan2(sqrt(a), sqrt(1 - a))
	double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

	// d = R * c, where d is the distance
	return R * c;
}

double Graph::toRadians(double degrees) {
	return degrees * PI / 180.0;
}
