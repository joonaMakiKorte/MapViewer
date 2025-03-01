#include "GraphLoader.hpp"
#include "ParseOSM.hpp"
#include "Binary.hpp"
#include <iostream>
#include <fstream>

bool GraphLoader::loadGraph(Graph& graph) {
	std::ifstream bin_file_stream(bin_file, std::ios::binary);

	// Try loading from binary first
	if (bin_file_stream.good()) {
		Binary::loadFromBinary(bin_file, graph);
		graph.createAdj(); // Create adjacency list
		return true;
	}

	// Initialize bbox with extreme values to ensure proper expansion
	graph.bbox.min_lat = std::numeric_limits<double>::max();
	graph.bbox.min_lon = std::numeric_limits<double>::max();
	graph.bbox.max_lat = std::numeric_limits<double>::lowest();
	graph.bbox.max_lon = std::numeric_limits<double>::lowest();
	
	// Load from OSM files
	for (const auto& osm_file : osm_files) {
		std::cout << "Loading " << osm_file << "...\n";
		ParseOSM::loadMap(osm_file, graph);
	}
	graph.createAdj(); // Create adjacency list

	// Save to binary
	Binary::saveToBinary(bin_file, graph);
	return false;
}
