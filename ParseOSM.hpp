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
    void static parseOSM(const std::string& filePath, Graph& graph, std::atomic<long long>& counter);

    // Helper for parseOSM to filter out certain ways
    // For example boat ways
    bool static isInvalidWay(const std::string& key, const std::string& value);

    // Create unique ID for graph edge
    long long static generateUniqueID(std::atomic<long long>& counter);

public:
    void static loadMap(const std::string& filePath, Graph& graph, std::atomic<long long>& counter) {
        parseOSM(filePath, graph, counter);
        graph.createAdj();
    }
};
