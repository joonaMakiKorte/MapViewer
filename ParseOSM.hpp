#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include "rapidxml.hpp"
#include "Graph.hpp"

class ParseOSM {

private:
    // Parse map information from input file to graph
    // Including nodes, edges and ways
    void static parseOSM(const std::string& filePath, Graph& graph);

public:
    void static loadMap(const std::string& filePath, Graph& graph) {
        parseOSM(filePath, graph);
        graph.createAdj();
    }
};
