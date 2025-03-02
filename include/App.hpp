#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "Graphics.hpp"
#include <memory>

constexpr sf::Color BG_COLOR = sf::Color(10, 10, 35); // Dark blue

class App {
public:
	App(Graph& graph);

	// Main event loop of application
	void run();

private:
	// Initial window resolution
	float window_width;
	float window_height;

	std::unique_ptr<Graphics> renderer; // Store pointer to renderer (Graphics class)
};

#endif 