# Map Visualizer and Route Planner

This C++ project is designed to visualize maps exported from OpenStreetMap and create routes between selected points on the map. The program converts `.osm` map data into a graph of nodes and edges, which is then visualized using SFML 3.0.0. The map data is parsed using RapidXML, and the graph is rendered as `VertexArray::Triangles`. The project supports parsing multiple `.osm` files, and the data is stored in a binary format for faster loading in subsequent runs. The project folder comes with two pre-converted maps — **`tampere.bin`** and **`helsinki.bin`**.

## Features

- **Map Visualization**: Convert `.osm` map data into a graph and visualize it using SFML.
- **Multiple Map Files**: Parse and merge multiple `.osm` files into a single graph.
- **Binary Data Storage**: Store parsed map data in binary format for quick loading.
- **Interactive Map**: Zoom, pan, and resize the map window.
- **Efficient Rendering**: Use a quadtree to render only the visible edges, ensuring smooth performance at 60 FPS.
- **Route Planning**: Calculate the shortest path between two points using the A* algorithm.
- **Haversine Distance**: Compute route distances using the Haversine formula and print them to the terminal.
- **CMake Build System**: Automatically downloads and links SFML during compilation.

## Screenshots

### Map Visualization
![Map Visualization](https://imgur.com/7yRS9bM.png)

### Route Planning
![Route Planning](https://imgur.com/485Lfte.png)

## Requirements

- **C++ Compiler**: Supports C++20 or later.
- **CMake**: Version 3.16 or later.
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

- **Loading Maps**: Place your `.osm` files in the `resources` directory. The program will automatically load and merge them into a single graph.
- **Navigating the Map**:
  - **Zoom**: Use the mouse scroll wheel.
  - **Pan**: Click and hold the mouse scroll wheel and drag the map.
  - **Resize**: Adjust the window size.
- **Creating Routes**: Click on two points on the map and then press Enter to calculate and visualize the shortest path between them. The route distance will be printed in the terminal.
  - **Pro-tip**: To de-select a point, click on it again ;) 

## Project Structure

- **`src/`**: Contains the main source code.
  - **`main.cpp`**: Entry point of the program.
  - **`GraphLoader.cpp`**: Load map data into a graph.
  - **`ParseOSM.cpp`**: Handles parsing of `.osm` files using RapidXML.
  - **`Binary.cpp`**: Handles binary data storage.
  - **`Graph.cpp`**: Manages the graph structure.
  - **`Algorithm.cpp`**: Handles the A* algorithm.
  - **`App.cpp`**: Manages the SFML window.
  - **`EventHandler.cpp`**: Handles the window events.
  - **`Graphics.cpp`**: Handles rendering using SFML.
  - **`Quadtree.cpp`**: Applies quadtree logic to rendering.
- **`include/`**: Header files.
- **`external/`**: RapidXML headers.
- **`resources/`**: Directory for storing `.osm` files and `.bin` files.
- **`CMakeLists.txt`**: CMake build configuration.

## Customizability

You have the option to download your own `.osm` files and visualize the map data with the program:
1. **Export map data from [OpenStreetMap](https://www.openstreetmap.org/)**
2. **Move the `.osm` files into the `resources/` directory**
3. **In `GraphLoader.hpp`, list the files into `osm_files` vector**
4. **Choose the name for the new `.bin` file to save the data into and modify the value of variable `bin_file` in `GraphLoader.hpp` to match that**
5. **Now you can build and run the program with your own imported map!**

## Dependencies

- **SFML**: Simple and Fast Multimedia Library for rendering.
- **RapidXML**: Fast XML parsing library.
- **CMake**: Build system for managing dependencies and compilation.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## Acknowledgments

- **[OpenStreetMap](https://www.openstreetmap.org/)**: For providing the map data. 
- **[SFML](https://www.sfml-dev.org/)** : For the graphics and window management.
- **[RapidXML](https://github.com/Fe-Bell/RapidXML)**: For efficient XML parsing.

## Contact

For any questions or suggestions, please open an issue or contact the maintainer directly.

---

Enjoy visualizing and planning routes on your maps! 🗺️
