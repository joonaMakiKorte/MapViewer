#include "Graph.hpp"

void Graph::addNode(Node node) {
	nodes[node.id] = node;
}

void Graph::addWay(Way way) {
	ways[way.id] = way;
}

void Graph::addEdge(long long id, Edge edge) {
	edges[id] = edge;
}

bool Graph::hasNode(long long id) {
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

const std::unordered_map<long long, Edge>& Graph::getEdges() const {
	return edges;
}

const std::unordered_map<long long, Node>& Graph::getNodes() const {
	return nodes;
}

double Graph::getDistance(const Node& from, const Node& to) {
	double dx = to.lon - from.lon;
	double dy = to.lat - from.lat;
	return sqrt(dx * dx + dy * dy);
}
