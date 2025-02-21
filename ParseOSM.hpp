#pragma once

#include "rapidxml.hpp"
#include "Graph.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <atomic>


class ParseOSM {

private:
    // Parse map information from input file to graph
    // Including nodes, edges and ways
    void static parseOSM(const std::string& filePath, Graph& graph, std::atomic<uint32_t>& counter);

    // Helper for parseOSM to filter out certain ways
    // For example boat ways
    bool static isInvalidWay(const std::string& key, const std::string& value);

    // Create unique 32-bit ID for graph edge
    uint32_t static generateUniqueID(std::atomic<uint32_t>& counter);

public:
    void static loadMap(const std::string& filePath, Graph& graph, std::atomic<uint32_t>& counter) {
        parseOSM(filePath, graph, counter);
        graph.createAdj();
    }
};
