#include "Graph.hpp"

void Graph::addNode(Node node) {
	nodes[node.id] = node;
}

void Graph::addWay(Way way) {
	ways[way.id] = way;
}

void Graph::addEdge(Edge edge, Edge reverse_edge) {
	edges.push_back(edge);
	edges.push_back(reverse_edge);
}

void Graph::addLimits(double lat_max, double lat_min, double lon_max, double lon_min) {
	max_lat = lat_max;
	min_lat = lat_min;
	max_lon = lon_max;
	min_lon = lon_min;
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

double Graph::getLimit(bool lat, bool max)
{
	return lat ? (max ? max_lat : min_lat) : (max ? max_lon : min_lon);
}

size_t Graph::nodeCount()
{
	return nodes.size();
}

std::span<const Edge> Graph::getEdges() {
	return { edges.data(), edges.size() };
}

double Graph::getDistance(const Node& from, const Node& to) {
	double dx = to.lon - from.lon;
	double dy = to.lat - from.lat;
	return sqrt(dx * dx + dy * dy);
}
