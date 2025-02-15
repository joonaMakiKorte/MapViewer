#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "ParseOSM.hpp"
#include <iostream>

int main()
{
    Graph graph;
    ParseOSM::loadMap("map.osm", graph);
    std::cout << graph.nodeCount() << std::endl;
}