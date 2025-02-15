#include "ParseOSM.hpp"

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

    std::vector<char> content(fileSize + 1);  // Use std::vector for safety
    file.read(content.data(), fileSize);
    content[fileSize] = '\0';  // Null-terminate the buffer
    file.close();

    // Parse the XML content
    rapidxml::xml_document<> doc;
    doc.parse<0>(content.data());

    // Get the root node
    rapidxml::optional_ptr<rapidxml::xml_node<char>> root_opt = doc.first_node("osm");
    if (!root_opt) {
        std::cerr << "Error: Invalid OSM file" << std::endl;
        return;
    }
    rapidxml::xml_node<>* root = root_opt.get();  // Extract raw pointer

    // Bounding box variables
    auto lat_max = std::make_unique<double>(std::numeric_limits<double>::min());
    auto lat_min = std::make_unique<double>(std::numeric_limits<double>::max());
    auto lon_max = std::make_unique<double>(std::numeric_limits<double>::min());
    auto lon_min = std::make_unique<double>(std::numeric_limits<double>::max());

    int nodeCount = 0;  // Track node count for debugging

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

            *lat_max = std::max(*lat_max, n.lat);
            *lat_min = std::min(*lat_min, n.lat);
            *lon_max = std::max(*lon_max, n.lon);
            *lon_min = std::min(*lon_min, n.lon);

            graph.addNode(n);
            nodeCount++;
        }

        // Parse Way elements
        else if (nodeName == "way") {
            if (!node->first_attribute("id")) continue;  // Skip invalid ways

            auto nodeRefs = std::make_unique<std::vector<long long>>();  // Heap allocation

            for (auto nd = node->first_node("nd"); nd; nd = nd->next_sibling("nd")) {
                nodeRefs->push_back(std::stoll(std::string(nd->first_attribute("ref")->value())));
            }

            Way w;
            w.id = std::stoll(std::string(node->first_attribute("id")->value()));

            for (size_t i = 1; i < nodeRefs->size(); ++i) {
                Edge edge{ (*nodeRefs)[i - 1], (*nodeRefs)[i] };
                Edge reverseEdge{ (*nodeRefs)[i], (*nodeRefs)[i - 1] };

                w.edges.push_back(edge);
                graph.addEdge(edge, reverseEdge);
            }
            graph.addWay(w);
        }
    }

    graph.addLimits(*lat_max, *lat_min, *lon_max, *lon_min);

    // Debug Output
    std::cout << "Nodes parsed: " << nodeCount << std::endl;
}