#include "MainWindow.hpp"

MainWindow::MainWindow(Graph& graph) :
    graph(graph),
    // Get the desktop resolution and initialize window resolution
    window_width(sf::VideoMode::getDesktopMode().size.x * 0.9f),
    window_height(sf::VideoMode::getDesktopMode().size.y * 0.9f)
{
    // Render with calculated scale
    renderer = std::make_unique<Graphics>(graph, window_width, window_height);
}

void MainWindow::run() {
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
        }

        // Apply the view
        window.setView(view);

        // Rendering
        window.clear(sf::Color::Black);
        renderer->render(window, view);
        window.display();
    }
}

void MainWindow::handleExit(sf::RenderWindow& window, const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::Closed>() ||
        (event->is<sf::Event::KeyPressed>() &&
            event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
        window.close();
    }
}

void MainWindow::handleResize(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view) {
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        std::cout << "New window size: " << resized->size.x << "x" << resized->size.y << std::endl;

        // Set new resolution parameters
        window_width = static_cast<float>(resized->size.x);
        window_height = static_cast<float>(resized->size.y);

        // Adjust the view size to match the new window size
        view.setSize({ window_width, window_height });
        view.setCenter({ window_width / 2.f, window_height / 2.f });

        // Rescale nodes
        renderer->rescaleNodes(window_width, window_height);
    }
}

void MainWindow::handleZoom(const std::optional<sf::Event>& event, sf::View& view) {
    if (const auto* scrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
        float zoom_factor = (scrolled->delta > 0) ? 0.9f : 1.1f;
        view.zoom(zoom_factor);
    }
}

void MainWindow::handlePanning(sf::RenderWindow& window, const std::optional<sf::Event>& event, sf::View& view) {
    // When mouse wheel is pressed, start panning
    if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>();
        pressed && pressed->button == sf::Mouse::Button::Middle) {
        isPanning = true;
        last_mouse_pos = window.mapPixelToCoords({ pressed->position.x, pressed->position.y });
    }

    // When mouse wheel is released, stop panning
    if (event->is<sf::Event::MouseButtonReleased>() &&
        event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Middle) {
        isPanning = false;
    }

    if (const auto* moving = event->getIf<sf::Event::MouseMoved>();
        moving && isPanning) {
        // Calculate the difference between the current mouse pos and last pos
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(mouse_pos, view) - last_mouse_pos;

        // Update the view center
        view.move(-delta); // Move the view in the opposite direction of the mouse drag

        last_mouse_pos = window.mapPixelToCoords(mouse_pos, view);
    }
}


