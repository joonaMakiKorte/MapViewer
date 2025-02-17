#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "ParseOSM.hpp"
#include "MainWindow.hpp"

int main()
{
    Graph graph;
    ParseOSM::loadMap("map.osm", graph);

    MainWindow app(graph);
    app.run();

    return 0;
}