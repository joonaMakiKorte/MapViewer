#include "Graph.hpp"

void Graph::addNode(Node node) {
	nodes[node.id] = node;
}

void Graph::addWay(Way way) {
	ways[way.id] = way;
}

void Graph::addEdge(Edge edge) {
	edges.push_back(edge);
}

void Graph::createAdj() {
	// Iterate over edges
	for (const auto& edge : edges) {
		// Get endpoint nodes and calculate weight
		const Node& from = nodes[edge.from];
		const Node& to = nodes[edge.to];
		double weight = getDistance(from, to);

		// Create adj list entry
		adj_list[edge.from].push_back({ edge.to,weight });
	}
}

const std::vector<Edge>& Graph::getEdges() const {
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
