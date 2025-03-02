#include "ParseOSM.hpp"
#include "rapidxml.hpp"
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

std::atomic<uint32_t> ParseOSM::counter(0); // Initialize counter to 0

void ParseOSM::parseOSM(const std::string& filePath, Graph& graph) {
    // Read the file into a dynamically allocated buffer
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    // Allocate memory on heap for file contents
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    auto content = std::make_unique<std::vector<char>>(fileSize + 1);  // Use std::vector for safety
    file.read(content->data(), fileSize);
    (*content)[fileSize] = '\0';  // Null-terminate the buffer
    file.close();

    // Parse the XML content
    auto doc = std::make_unique<rapidxml::xml_document<>>();
    doc->parse<0>(content->data());

    // Validate file
    rapidxml::optional_ptr<rapidxml::xml_node<char>> root_opt = doc->first_node("osm");
    if (!root_opt) {
        std::cerr << "Error: Invalid OSM file" << std::endl;
        return;
    }
    rapidxml::xml_node<>* root = root_opt.get();  // Extract raw pointer

    // Find map bounds
    rapidxml::optional_ptr<rapidxml::xml_node<char>> bounds_node_opt = root_opt->first_node("bounds");
    if (bounds_node_opt) {
        try {
            rapidxml::xml_node<>* bounds_node = bounds_node_opt.get();  // Extract raw pointer
            // Extract and convert attributes
            double min_lat = std::stod(std::string(bounds_node->first_attribute("minlat")->value()));
            double min_lon = std::stod(std::string(bounds_node->first_attribute("minlon")->value()));
            double max_lat = std::stod(std::string(bounds_node->first_attribute("maxlat")->value()));
            double max_lon = std::stod(std::string(bounds_node->first_attribute("maxlon")->value()));

            // Expand bounding box to include new file's bounds
            graph.bbox.min_lat = std::min(graph.bbox.min_lat, min_lat);
            graph.bbox.min_lon = std::min(graph.bbox.min_lon, min_lon);
            graph.bbox.max_lat = std::max(graph.bbox.max_lat, max_lat);
            graph.bbox.max_lon = std::max(graph.bbox.max_lon, max_lon);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: Invalid <bounds> in OSM file." << std::endl;
            return;
        }
    }
    else {
        std::cerr << "Error: No <bounds> found in OSM file." << std::endl;
        return;
    }

    // Iterate over child nodes
    for (auto node = root->first_node(); node; node = node->next_sibling()) {
        std::string nodeName(node->name());

        // Parse Node elements
        if (nodeName == "node") {
            if (!node->first_attribute("id") || !node->first_attribute("lat") || !node->first_attribute("lon")) {
                continue;  // Skip invalid nodes
            }

            Graph::Node n;
            int64_t node_id = std::stoll(std::string(node->first_attribute("id")->value())); // Cast id to int64_t
            // Cast coordinates to double
            n.lat = std::stod(std::string(node->first_attribute("lat")->value()));
            n.lon = std::stod(std::string(node->first_attribute("lon")->value()));

            // Add to graph if passes filter
            if (graph.bbox.contains(n.lat, n.lon)) {
                graph.addNode(node_id, n);
            }
        }

        // Parse Way elements
        else if (nodeName == "way") {
            if (!node->first_attribute("id")) continue;  // Skip invalid ways

            // Check all tags in the way to make sure way passes our filter
            bool isValid = true; // Flag to detect invalid way
            for (auto tag = node->first_node("tag"); tag; tag = tag->next_sibling("tag")) {
                std::string key = std::string(tag->first_attribute("k")->value());
                std::string value = std::string(tag->first_attribute("v")->value());

                // Use helper function to determine if this way is invalid
                if (!isValidWay(key, value)) {
                    isValid = false;
                    break;
                }
            }
            if (!isValid) continue;

            // Get all node references in the way
            auto node_refs = std::make_unique<std::vector<int64_t>>();  // Heap allocation
            for (auto nd = node->first_node("nd"); nd; nd = nd->next_sibling("nd")) {
                node_refs->push_back(std::stoll(std::string(nd->first_attribute("ref")->value())));
            }

            for (size_t i = 1; i < node_refs->size(); ++i) {
				// Get source and target nodes for edge
				int64_t from = (*node_refs)[i - 1];
                int64_t to = (*node_refs)[i];
                
                // Ensure both source and target nodes of the edge exists
                // Meaning neither have been filtered out
				// Also check that the edge does not already exist
                if (graph.hasNode(from) && graph.hasNode(to) && !graph.hasEdge(from,to)) {
                    // Generate id for edge
                    uint32_t edge_id = generateUniqueID();
					graph.addEdge(edge_id, {from,to}); // Add edge to graph
                }
            }
        }
    }
}

bool ParseOSM::isValidWay(const std::string& key, const std::string& value) {
    static const std::unordered_set<std::string> non_routable_keys = {
        "boundary", "building", "landuse", "natural", "waterway", "railway"
    };

    if (non_routable_keys.count(key)) return false;

    // Exclude private/proposed access restrictions
    if (key == "access" && (value == "private" || value == "no")) return false;
    if (key == "highway" && value == "proposed") return false;

    // Exclude boat routes, ferries, and other non-road navigational routes
    if (key == "route" && (value == "boat" || value == "ferry" || value == "ship" || value == "seaway")) return false;
    if (key == "highway" && value == "ferry") return false;
    if (key == "service" && value == "ferry") return false;
    if (key == "motorboat" && (value == "yes" || value == "designated")) return false;
    if (key == "transport_mode" && (value == "boat" || value == "ship")) return false;

    return true;
}

uint32_t ParseOSM::generateUniqueID() {
    // Get the current time in nanoseconds since the epoch
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

    // Use only the lower 32 bits of the timestamp
    uint32_t nanos_32 = static_cast<uint32_t>(nanos & 0xFFFFFFFF);

    // Combine the lower 16 bits of the counter with the lower 32 bits of the timestamp
    uint32_t unique_ID = ((nanos_32 & 0xFFFF) << 16) | (counter.fetch_add(1, std::memory_order_relaxed) & 0xFFFF);

    return unique_ID;
}