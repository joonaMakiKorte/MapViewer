#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "Graphics.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

class EventHandler {
public: 
	EventHandler(sf::RenderWindow& window, sf::View& view, Graphics& renderer);

	// Handles all window events
	void handleEvent(const std::optional<sf::Event>& event);

private:
	// Handle exit by closing window or esc
	void handleExit(const std::optional<sf::Event>& event);

	// Handle window resizing
	void handleResize(const std::optional<sf::Event>& event);

	// Zoom window view with mouse wheel scroll
	void handleZoom(const std::optional<sf::Event>& event);

	// Move the view by pressing mouse wheen and dragging the mouse (panning)
	void handlePanning(const std::optional<sf::Event>& event);

	// Highlight a node by clicking on it
	void handleSelection(const std::optional<sf::Event>& event);

	// Find route between two nodes by pressing enter
	void handleRoute(const std::optional<sf::Event>& event);

private:
	// Store references to main window and renderer
	sf::RenderWindow& window;
	sf::View& view;
	Graphics& renderer;

	float current_zoom; // Keep track of current zoom level
	bool is_panning; // Flag to track if currently panning
	sf::Vector2f last_mouse_pos; // Track last position of mouse
};

#endif