#pragma once

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "Graphics.hpp"
#include <iostream>

class MainWindow {
public:
	MainWindow(Graph& graph);
	void run();

private:
	Graph& graph;
	Graphics renderer;
};

