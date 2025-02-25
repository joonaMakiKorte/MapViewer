#include "App.hpp"
#include "GraphLoader.hpp"

int main() {
	Graph graph;
	GraphLoader::loadGraph(graph, "tampere.osm", "tampere.bin");

    App app(graph);
    app.run();
}