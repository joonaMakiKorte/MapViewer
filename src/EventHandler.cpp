#include "EventHandler.hpp"

EventHandler::EventHandler(sf::RenderWindow& win, sf::View& v, Graphics& rend) : 
	window(win), view(v), renderer(rend), current_zoom(1.0f), is_panning(false)
{}

bool EventHandler::handleEvent(const std::optional<sf::Event>&event) {
    handleExit(event);
    bool resized = handleResize(event);
    bool zoomed = handleZoom(event);
    bool moved = handlePanning(event);
    bool selected = handleSelection(event);
    bool searched = handleRoute(event);

    // Return bool indicating if changes were made in the window
    return resized || zoomed || moved || selected || searched;
}

void EventHandler::handleExit(const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::Closed>() ||
        (event->is<sf::Event::KeyPressed>() &&
            event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
        window.close();
    }
}

bool EventHandler::handleResize(const std::optional<sf::Event>& event) {
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        // Resize window
        window.setSize({ resized->size.x, resized->size.y });

        // Reset zoom
        current_zoom = 1.0f;

        // Adjust the view size to match the new window size
        view.setSize({ static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
        view.setCenter({ view.getSize().x / 2.f, view.getSize().y / 2.f });

        // Rescale nodes
        renderer.rescaleGraphics(resized->size.x, resized->size.y);
        return true;
    }
    return false;
}

bool EventHandler::handleZoom(const std::optional<sf::Event>& event) {
    static constexpr float max_zoom = 0.05f;   // Prevent excessive zoom-in
    static constexpr float min_zoom = 1.0f;   // No zoom out beyond initial size

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
        return true;
    }
    return false;
}

bool EventHandler::handlePanning(const std::optional<sf::Event>& event) {
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
        if (new_left >= 0 && new_right <= window.getSize().x &&
            new_top >= 0 && new_bottom <= window.getSize().y) {
            // Update the view center
            view.move(-delta); // Move the view in the opposite direction of the mouse drag

            last_mouse_pos = window.mapPixelToCoords(mouse_pos, view);
            return true;
        }
    }
    return false;
}

bool EventHandler::handleSelection(const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::MouseButtonPressed>() &&
        event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        renderer.selectNode(window, view, mouse_pos);
        return true;
    }
    return false;
}

bool EventHandler::handleRoute(const std::optional<sf::Event>& event) {
    if (event->is<sf::Event::KeyPressed>() &&
        event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Enter) {
        double distance = 0;
        renderer.findRoute();
        return true;
    }
    return false;
}
