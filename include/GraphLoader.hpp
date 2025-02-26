#pragma once

#include "ParseOSM.hpp"
#include "Binary.hpp"

// List of OSM files to load
const std::vector<std::string> osm_files = {
		"resources/tampere1.osm",
		"resources/tampere2.osm",
		"resources/tampere3.osm",
		"resources/tampere4.osm"
};
// Binary file to save/load
const std::string bin_file = "resources/map.bin";

class GraphLoader {

public:
	static bool loadGraph(Graph& graph);
};

