#include "App.hpp"

App::App(Graph& graph) :
    // Get the desktop resolution and initialize window resolution
    window_width(sf::VideoMode::getDesktopMode().size.x * 0.9f),
    window_height(sf::VideoMode::getDesktopMode().size.y * 0.9f),
    current_zoom(1.0f), is_panning(false)
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

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Handle events
            handleExit(window, event);
            handleResize(window, event, view);
            handleZoom(event, view); 
            handlePanning(window, event, view);
			handleSelection(window, event, view);
			handleRoute(event);
        }

        // Apply the view
        window.setView(view);

        // Rendering
        window.clear(sf::Color::Black);
        renderer->render(window, view);
        window.display();
    }
}

void App::handleExit(sf::RenderWindow& window, const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::Closed>() ||
        (event->is<sf::Event::KeyPressed>() &&
            event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
        window.close();
    }
}

void App::handleResize(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view) {
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        // Set new resolution parameters
        window_width = static_cast<float>(resized->size.x);
        window_height = static_cast<float>(resized->size.y);

        // Reset zoom
        current_zoom = 1.0f;

        // Adjust the view size to match the new window size
        view.setSize({ window_width, window_height });
        view.setCenter({ window_width / 2.f, window_height / 2.f });

        // Rescale nodes
        renderer->rescaleGraphics(window_width, window_height);
    }
}

void App::handleZoom(const std::optional<sf::Event>&event, sf::View & view) {
    static float max_zoom = 0.05f;   // Prevent excessive zoom-in
    static float min_zoom = 1.0f;   // No zoom out beyond initial size

    if (const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
        float zoom_factor = (scrolled->delta > 0) ? 0.9f : 1.1f;

        // Get current zoom level
        current_zoom *= zoom_factor;

        // Clamp zoom level
        if (current_zoom < max_zoom) {
            zoom_factor = max_zoom / (current_zoom / zoom_factor);
            current_zoom = max_zoom;
        }
        else if (current_zoom > min_zoom) {
            zoom_factor = min_zoom / (current_zoom / zoom_factor);
            current_zoom = min_zoom;
        }

        view.zoom(zoom_factor); // Apply zoom
    }
}

void App::handlePanning(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view) {
    // When mouse wheel is pressed, start panning
    if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>();
        pressed && pressed->button == sf::Mouse::Button::Middle) {
        is_panning = true;
        last_mouse_pos = window.mapPixelToCoords({ pressed->position.x, pressed->position.y });
    }

    // When mouse wheel is released, stop panning
    if (event->is<sf::Event::MouseButtonReleased>() &&
        event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Middle) {
        is_panning = false;
    }

    // Apply panning view movement
    if (const auto* moving = event->getIf<sf::Event::MouseMoved>();
        moving && is_panning) {
        // Calculate the difference between the current mouse pos and last pos
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(mouse_pos, view) - last_mouse_pos;

        // Get the current view bounds
        sf::Vector2f new_center = view.getCenter() - delta; // Apply movement

        float half_width = view.getSize().x / 2;
        float half_height = view.getSize().y / 2;

        float new_left = new_center.x - half_width;
        float new_right = new_center.x + half_width;
        float new_top = new_center.y - half_height;
        float new_bottom = new_center.y + half_height;

        // Clamp the view position to stay within the graph bounds
        if (new_left >= 0 && new_right <= window_width &&
            new_top >= 0 && new_bottom <= window_height) {
            // Update the view center
            view.move(-delta); // Move the view in the opposite direction of the mouse drag

            last_mouse_pos = window.mapPixelToCoords(mouse_pos, view);
        }
    }
}

void App::handleSelection(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view) {
	if (event->is<sf::Event::MouseButtonPressed>() &&
		event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
		renderer->selectNode(window, view, mouse_pos);
	}
}

void App::handleRoute(const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::KeyPressed>() &&
        event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter) {
        double distance = 0;
        if (renderer->findRoute(distance)) {
            if (distance > 0) {
                std::cout << "Distance: " << distance << " meters" << std::endl;
            }
            else {
                std::cout << "No path found!" << std::endl;
            }
        }
        else {
            std::cerr << "Both from and target nodes must be selected!" << std::endl;
        }
    }
}


