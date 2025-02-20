#include "ParseOSM.hpp"

void ParseOSM::parseOSM(const std::string& filePath, Graph& graph, std::atomic<long long>& counter) {
    // Read the file into a dynamically allocated buffer
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    // Allocate memory on heap for file contents
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> content(fileSize + 1);  // Use std::vector for safety
    file.read(content.data(), fileSize);
    content[fileSize] = '\0';  // Null-terminate the buffer
    file.close();

    // Parse the XML content
    rapidxml::xml_document<> doc;
    doc.parse<0>(content.data());

    // Validate file
    rapidxml::optional_ptr<rapidxml::xml_node<char>> root_opt = doc.first_node("osm");
    if (!root_opt) {
        std::cerr << "Error: Invalid OSM file" << std::endl;
        return;
    }
    
    // Find map bounds
    rapidxml::optional_ptr<rapidxml::xml_node<char>> boundsNodeOpt = root_opt->first_node("bounds");
    if (boundsNodeOpt) {
        rapidxml::xml_node<>* boundsNode = boundsNodeOpt.get();  // Extract raw pointer

        // Extract and convert attributes
        graph.bbox.min_lat = std::stod(std::string(boundsNode->first_attribute("minlat")->value()));
        graph.bbox.min_lon = std::stod(std::string(boundsNode->first_attribute("minlon")->value()));
        graph.bbox.max_lat = std::stod(std::string(boundsNode->first_attribute("maxlat")->value()));
        graph.bbox.max_lon = std::stod(std::string(boundsNode->first_attribute("maxlon")->value()));
    }
    else {
        std::cerr << "Error: No <bounds> found in OSM file." << std::endl;
        return;
    }

    rapidxml::xml_node<>* root = root_opt.get();  // Extract raw pointer

    // Iterate over child nodes
    for (auto node = root->first_node(); node; node = node->next_sibling()) {
        std::string nodeName(node->name());

        // Parse Node elements
        if (nodeName == "node") {
            if (!node->first_attribute("id") || !node->first_attribute("lat") || !node->first_attribute("lon")) {
                continue;  // Skip invalid nodes
            }

            Node n;
            n.id = std::stoll(std::string(node->first_attribute("id")->value()));
            n.lat = std::stod(std::string(node->first_attribute("lat")->value()));
            n.lon = std::stod(std::string(node->first_attribute("lon")->value()));

            // Add to graph if passes filter
            if (graph.bbox.contains(n.lat, n.lon)) {
                graph.addNode(n);
            }
        }

        // Parse Way elements
        else if (nodeName == "way") {
            if (!node->first_attribute("id")) continue;  // Skip invalid ways

            auto nodeRefs = std::make_unique<std::vector<long long>>();  // Heap allocation

            // Check all tags in the way to make sure way passes our filter
            bool isValid = true; // Flag to detect invalid way
            for (auto tag = node->first_node("tag"); tag; tag = tag->next_sibling("tag")) {
                std::string key = std::string(tag->first_attribute("k")->value());
                std::string value = std::string(tag->first_attribute("v")->value());

                // Use helper function to determine if this way is invalid
                if (isInvalidWay(key, value)) {
                    isValid = false;
                    break;
                }
            }
            if (!isValid) continue;

            for (auto nd = node->first_node("nd"); nd; nd = nd->next_sibling("nd")) {
                nodeRefs->push_back(std::stoll(std::string(nd->first_attribute("ref")->value())));
            }

            Way w;
            w.id = std::stoll(std::string(node->first_attribute("id")->value()));

            for (size_t i = 1; i < nodeRefs->size(); ++i) {
                Edge edge{ (*nodeRefs)[i - 1], (*nodeRefs)[i] };

                // Ensure both source and target nodes of the edge exists
                // Meaning neither have been filtered out
                if (graph.hasNode(edge.from) && graph.hasNode(edge.to)) {
                    // Generate id for edge
                    long long edge_id = generateUniqueID(counter);

                    graph.addEdge(edge_id, edge);
                    w.edges.push_back(edge_id);
                }
            }
            graph.addWay(w);
        }
        
    }
}

bool ParseOSM::isInvalidWay(const std::string& key, const std::string& value) {
    return (key == "waterway") ||
        (key == "route" && value == "ferry") ||
        (key == "building");
}

long long ParseOSM::generateUniqueID(std::atomic<long long>& counter) {
    // Get the current time in nanoseconds since the epoch
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();

    // Combine timestamp with the counter to create the unique ID
    long long uniqueID = (nanos << 16) | (counter.fetch_add(1, std::memory_order_relaxed) & 0xFFFF);

    return uniqueID;
}
