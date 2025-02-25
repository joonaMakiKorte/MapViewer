#pragma once

#include "ParseOSM.hpp"
#include "Binary.hpp"

class GraphLoader {

public:
	static bool loadGraph(Graph& graph, const std::string& osm_file, const std::string& bin_file);
};

