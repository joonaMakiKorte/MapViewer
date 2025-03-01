#ifndef GRAPHLOADER_H
#define GRAPHLOADER_H

#include "Graph.hpp"
#include <string>
#include <filesystem>

// Base path to resource directory (CMAKE_SOURCE_DIR is set in CMake)
const std::filesystem::path RESOURCE_PATH = std::filesystem::path(CMAKE_SOURCE_DIR) / "resources";

// List of OSM files to load (absolute paths)
const std::vector<std::string> osm_files = {
    (std::filesystem::path(RESOURCE_PATH) / "tampere1.osm").string(),
    (std::filesystem::path(RESOURCE_PATH) / "tampere2.osm").string(),
    (std::filesystem::path(RESOURCE_PATH) / "tampere3.osm").string(),
    (std::filesystem::path(RESOURCE_PATH) / "tampere4.osm").string()
};

// Binary file to save/load (absolute path)
const std::string bin_file = (std::filesystem::path(RESOURCE_PATH) / "map.bin").string();

class GraphLoader {

public:
	static bool loadGraph(Graph& graph);
};

#endif