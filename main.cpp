#include "App.hpp"
#include "GraphLoader.hpp"

int main() {
	Graph graph;

	std::vector<std::string> osm_files = {
		"tampere1.osm",
		"tampere2.osm",
		"tampere3.osm",
		"tampere4.osm"
	};

	GraphLoader::loadGraph(graph, osm_files, "map.bin");

    App app(graph);
    app.run();
}