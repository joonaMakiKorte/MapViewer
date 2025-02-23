#include "App.hpp"
#include "ParseOSM.hpp"


int main()
{
	// Load map data
	Graph graph;
	ParseOSM::loadMap("map.osm", graph);

    App app(graph);
    app.run();
}