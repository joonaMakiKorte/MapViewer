#pragma once

#include "ParseOSM.hpp"
#include "Binary.hpp"

// List of OSM files to load
const std::vector<std::string> osm_files = {
		"tampere1.osm",
		"tampere2.osm",
		"tampere3.osm",
		"tampere4.osm"
};
// Binary file to save/load
const std::string bin_file = "map.bin";

class GraphLoader {

public:
	static bool loadGraph(Graph& graph);
};

