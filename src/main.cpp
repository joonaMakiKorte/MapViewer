#include "App.hpp"
#include "GraphLoader.hpp"

int main() {
	Graph graph;
	GraphLoader::loadGraph(graph);

    App app(graph);
    app.run();
}