#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "Graph.h"

using namespace rapidxml;

class ParseMap
{
public:
    // Parse map information from input file to graph
    // Including nodes, edges and ways
    void parseOSM(const std::string& filePath, Graph& graph);
    // Create adjacency list
    void createAdj(Graph& graph); 

private:
    // Euclidean distance between two nodes
    double getDistance(const Node& from, const Node& to);
};
