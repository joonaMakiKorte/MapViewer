#pragma once

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "Graphics.hpp"
#include <iostream>
#include <iomanip>
#include <memory>

class App {
public:
	App(Graph& graph);

	void run();

private:
	// Handle exit by closing window or esc
	void handleExit(sf::RenderWindow& window, const std::optional<sf::Event>& event);

	// Handle window resizing
	void handleResize(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view);

	// Zoom window view with mouse wheel scroll
	void handleZoom(const std::optional<sf::Event>& event, sf::View& view);

	// Move the view by pressing mouse wheen and dragging the mouse (panning)
	void handlePanning(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view);

	// Highlight a node by clicking on it
	void handleSelection(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view);

	// Find route between two nodes by pressing enter
	void handleRoute(const std::optional<sf::Event>& event);

private:
	std::unique_ptr<Graphics> renderer;

	// Initial window size
	float window_width;
	float window_height;

	float current_zoom; // Keep track of current zoom level

	bool is_panning; // Flag to track if we are panning
	sf::Vector2f last_mouse_pos; // Track last mouse position
};

