#ifndef GRAPHLOADER_H
#define GRAPHLOADER_H

#include "Graph.hpp"
#include <string>
#include <filesystem>

// Base path to resource directory (CMAKE_SOURCE_DIR is set in CMake)
const std::filesystem::path RESOURCE_PATH = std::filesystem::path(CMAKE_SOURCE_DIR) / "resources";

// Creating your own map:
// 
// Input your own .osm files in this header for new map creation.
//
// Enter your filenames in format (std::filesystem::path(RESOURCE_PATH) / "filename.osm").string()
// where filename.osm is your downloaded .osm file in the /resources folder!!!
// 
// List your .osm files in the osm_files vector with a comma in between!!! An example osm_files vector:
// 
// const std::vector<std::string> osm_files = {
// (std::filesystem::path(RESOURCE_PATH) / "helsinki1.osm").string(),
// (std::filesystem::path(RESOURCE_PATH) / "helsinki2.osm").string(),
// (std::filesystem::path(RESOURCE_PATH) / "helsinki3.osm").string(),
// (std::filesystem::path(RESOURCE_PATH) / "helsinki4.osm").string()
// };
// 
// Remember to also change the bin_file variable to save your map into a new .bin file!!!
// 
// The resources folder comes with two pre-loaded .bin files: tampere.bin and helsinki.bin
// This means that the map will be loaded fast in the first run, since the .osm files are already parsed,
// so be ready for longer loading times when parsing new .osm files!!!
// 
// If you don't want to download your own .osm files but instead want to view the helsinki.bin map,
// change the filename in bin_file to helsinki.bin!!!
// 

///////////////////////////////
// ENTER YOUR OSM FILES HERE //
///////////////////////////////

// List of OSM files to load (absolute paths)
const std::vector<std::string> osm_files = {
};

//////////////////////////////
// ENTER YOUR BIN FILE HERE //
//////////////////////////////
 
// Binary file to save/load (absolute path)
const std::string bin_file = (std::filesystem::path(RESOURCE_PATH) / "tampere.bin").string();

class GraphLoader {
public:
	static bool loadGraph(Graph& graph);
};

#endif