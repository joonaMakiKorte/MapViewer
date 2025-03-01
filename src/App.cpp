#include "App.hpp"
#include "EventHandler.hpp"

App::App(Graph& graph) : 
    // Get the desktop resolution and initialize window resolution
    window_width(sf::VideoMode::getDesktopMode().size.x * 0.9f),
    window_height(sf::VideoMode::getDesktopMode().size.y * 0.9f)
{
    // Render with calculated scale
    renderer = std::make_unique<Graphics>(graph, window_width, window_height);
}

void App::run() {
    // Create a window with the same pixel depth as the desktop mode
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(window_width), static_cast<unsigned int>(window_height) },
        desktop.bitsPerPixel), "Map Viewer", sf::Style::Default);
    window.setFramerateLimit(60);  // Smoother performance

    // Set up initial view
    sf::View view;
    view.setSize({ window_width, window_height });
    view.setCenter({ window_width / 2.0f, window_height / 2.0f });

    // Initialize event handler for window
    EventHandler event_handler(window, view, *renderer);

    // Track if updates occurred and render only if so
    bool should_render = true; // Initialized as true since we need the initial render

    // Start event loop
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Handle events
            if (event_handler.handleEvent(event)) {
                should_render = true;
            }
        }

        // Render only when necessary
        if (should_render) {
            // Apply view and render
            window.setView(view);
            window.clear(sf::Color(10, 10, 35)); // Set a dark blue background
            renderer->render(window, view);
            window.display();
        }
        should_render = false;
    }
}