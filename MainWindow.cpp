#include "MainWindow.hpp"

MainWindow::MainWindow(Graph& graph) :
	graph(graph), renderer(graph)
{}

void MainWindow::run() {
    // Create a window with the same pixel depth as the desktop mode
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({ 1024, 768 }, desktop.bitsPerPixel),
        "Map Viewer", sf::Style::Default);
    window.setFramerateLimit(60);  // Smoother performance

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // Handle window close of esc
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                    event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                window.close();
            }

            // Handle window resize
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                std::cout << "New window size: " << resized->size.x << "x" << resized->size.y << std::endl;
            }
        }
        window.clear(sf::Color::Black);
        renderer.render(window);
        window.display();
    }
}


