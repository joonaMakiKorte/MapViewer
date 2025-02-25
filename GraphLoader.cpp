#include "GraphLoader.hpp"

bool GraphLoader::loadGraph(Graph& graph, const std::string& osm_file, const std::string& bin_file) {
	std::ifstream bin_file_stream(bin_file, std::ios::binary);
	if (bin_file_stream.good()) {
		Binary::loadFromBinary(bin_file, graph);
		return true;
	}
	else {
		ParseOSM::loadMap(osm_file, graph);
		Binary::saveToBinary(bin_file, graph);
		return false;
	}
}
