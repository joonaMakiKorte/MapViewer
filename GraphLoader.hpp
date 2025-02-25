#pragma once

#include "ParseOSM.hpp"
#include "Binary.hpp"

class GraphLoader {

public:
	static bool loadGraph(Graph& graph, const std::vector<std::string>& osm_files, const std::string& bin_file);
};

