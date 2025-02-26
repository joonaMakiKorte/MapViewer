#pragma once

#include "rapidxml.hpp"
#include "Graph.hpp"
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <atomic>


class ParseOSM {
private: 
	static std::atomic<uint32_t> counter; // Counter for generating unique edge IDs

private:
    // Parse map information from input file to graph
    // Including nodes, edges and ways
    void static parseOSM(const std::string& file_path, Graph& graph);

    // Helper for parseOSM to filter out certain ways
    // For example boat ways
    bool static isValidWay(const std::string& key, const std::string& value);

    // Create unique 32-bit int ID for graph edge
    uint32_t static generateUniqueID();

public:
    void static loadMap(const std::string& file_path, Graph& graph) {
        parseOSM(file_path, graph);
    }
};
