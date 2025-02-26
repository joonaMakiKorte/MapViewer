# Map Visualizer and Route Planner

This C++ project is designed to visualize maps exported from OpenStreetMap and create routes between selected points on the map. The program converts `.osm` map data into a graph of nodes and edges, which is then visualized using SFML 3.0.0. The map data is parsed using RapidXML, and the graph is rendered as `VertexArray::Lines`. The project supports parsing multiple `.osm` files, and the data is stored in a binary format for faster loading in subsequent runs.

## Features

- **Map Visualization**: Convert `.osm` map data into a graph and visualize it using SFML.
- **Multiple Map Files**: Parse and merge multiple `.osm` files into a single graph.
- **Binary Data Storage**: Store parsed map data in binary format for quick loading.
- **Interactive Map**: Zoom, pan, and resize the map window.
- **Efficient Rendering**: Use a quadtree to render only the visible edges, ensuring smooth performance at 60 FPS.
- **Route Planning**: Calculate the shortest path between two points using the A* algorithm.
- **Haversine Distance**: Compute route distances using the Haversine formula and print them to the terminal.
- **CMake Build System**: Automatically downloads and links SFML during compilation.

## Requirements

- **C++ Compiler**: Supports C++20 or later.
- **CMake**: Version 3.14 or later.
- **SFML**: Version 3.0.0 (automatically downloaded during build).
- **RapidXML**: Header-only library included in the project.

## Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/joonaMakiKorte/MapViewer.git
   cd MapViewer
   ```

2. **Build the Project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the Program**:
   ```bash
   ./MapViewer
   ```

## Usage

- **Loading Maps**: Place your `.osm` files in the `maps` directory. The program will automatically load and merge them into a single graph.
- **Navigating the Map**:
  - **Zoom**: Use the mouse scroll wheel.
  - **Pan**: Click and drag the map.
  - **Resize**: Adjust the window size.
- **Creating Routes**: Click on two points on the map to calculate and visualize the shortest path between them. The route distance will be printed in the terminal.

## Project Structure

- **`src/`**: Contains the main source code.
  - **`main.cpp`**: Entry point of the program.
  - **`App.cpp`**: Manages the SFML window and events.
  - **`ParseOSM.cpp`**: Handles parsing of `.osm` files using RapidXML.
  - **`Graph.cpp`**: Manages the graph structure.
  - **`Graphics.cpp`**: Handles rendering using SFML and quadtree logic.
- **`include/`**: Header files.
- **`external/`**: RapidXML headers.
- **`resources/`**: Directory for storing `.osm` files and the `.bin` file.
- **`CMakeLists.txt`**: CMake build configuration.

## Dependencies

- **SFML**: Simple and Fast Multimedia Library for rendering.
- **RapidXML**: Fast XML parsing library.
- **CMake**: Build system for managing dependencies and compilation.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **OpenStreetMap**: For providing the map data.
- **SFML**: For the graphics and window management.
- **RapidXML**: For efficient XML parsing.

## Contact

For any questions or suggestions, please open an issue or contact the maintainer directly.

---

Enjoy visualizing and planning routes on your maps! 🗺️
