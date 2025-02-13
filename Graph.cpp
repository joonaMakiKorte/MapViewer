#include "Graph.h"

Node* Graph::addNode(int id, float x, float y)
{
    auto node = std::make_unique<Node>(id, x, y);
    Node* rawPtr = node.get(); // Get pointer to node
    nodes.push_back(std::move(node));
    return rawPtr;
}

Edge* Graph::addEdge(Node* from, Node* to, int weight = 1) {
    auto edge = std::make_unique<Edge>(from, to, weight);
    Edge* rawPtr = edge.get();
    edges.push_back(std::move(edge));
    from->neighbors.push_back(to); // Add the connection to the source node
    return rawPtr;
}

void Graph::removeEdge(Edge* edge) 
{
    // Remove the edge from the source node's neighbors
    auto& neighbors = edge->from->neighbors;
    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), edge->to), neighbors.end());

    // Remove the edge from the graph's edges container
    auto it = std::find_if(edges.begin(), edges.end(),
        [edge](const std::unique_ptr<Edge>& e) { return e.get() == edge; });
    if (it != edges.end()) {
        edges.erase(it);
    }
}
