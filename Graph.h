#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
class Edge;

class Node
{
public:
	int id;
	float x, y; // Node coordinates
	std::vector<Node*> neighbors; // Non-owning relationship
	std::vector<Edge*> edges; // Non-owning relationship
};

class Edge
{
public:
	Node* from; // Source
	Node* to; // Target
	int weight; // For weighted graph traversal 
	sf::VertexArray line; // Visual representation

	Edge(Node* from, Node* to, int weight = 1) :
		from(from), to(to), weight(weight) {}
};

class Graph
{
public:
	Graph() {};

	Node* addNode(int id, float x, float y);
	Edge* addEdge(Node* from, Node* to, int weight = 1);

	void removeEdge(Edge* edge);

private:
	std::vector<std::unique_ptr<Node>> nodes; // Owning relationship
	std::vector<std::unique_ptr<Edge>> edges; // Owning relationship
};

