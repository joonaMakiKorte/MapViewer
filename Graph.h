#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

class Edge;

struct Node
{
	long long id;
	double lat; // Latitude
	double lon; // Longitude
};

class Edge
{
public:
	long long from; // Source
	long long to; // Target
	sf::VertexArray line; // Visual representation
};

struct Way
{
	long long id;
	std::vector<Edge> edges; 
};

class Graph
{
public:
	std::unordered_map<long long, Node> nodes; // ID to node
	std::unordered_map<long long, Way> ways; // ID to way
	std::unordered_map<long long, std::vector<std::pair<long long, double>>> adjList; // Adjacency list for traversal
	std::vector<Edge> edges;
};

