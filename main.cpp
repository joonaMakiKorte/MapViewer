#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "ParseOSM.hpp"
#include "MainWindow.hpp"

int main()
{
    // Ensure thread safety
    std::atomic<uint32_t> counter{ 0 };

    Graph graph;
    ParseOSM::loadMap("map.osm", graph, counter);

    MainWindow app(graph);
    app.run();

    return 0;
}