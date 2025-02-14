#include "ParseMap.h"

void ParseMap::parseOSM(const std::string& filePath, Graph& graph) {
    // Read the file into a string
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    // Parse the XML content
    xml_document<> doc;
    doc.parse<0>(&content[0]);

    // Get the root node
    auto root_opt = doc.first_node("osm");
    if (!root_opt) {
        std::cerr << "Error: Invalid OSM file" << std::endl;
        return;
    }
    xml_node<>* root = root_opt.get(); // Extract the raw pointer


    // Iterate over child nodes
    for (auto node_opt = root->first_node(); node_opt; node_opt = node_opt->next_sibling()) {
        xml_node<>* node = node_opt.get(); // Extract the raw pointer
        std::string nodeName(node->name());

        // Parse Node elements
        if (nodeName == "node") {
            Node n;
            n.id = stoll(std::string(node->first_attribute("id")->value()));
            n.lat = stod(std::string(node->first_attribute("lat")->value()));
            n.lon = stod(std::string(node->first_attribute("lon")->value()));
            graph.nodes[n.id] = n;
        }

        // Parse Way elements
        else if (nodeName == "way") {
            std::vector<long long> nodeRefs;

            // Iterate over nd (node reference) elements
            for (auto nd_opt = node->first_node("nd"); nd_opt; nd_opt = nd_opt->next_sibling("nd")) {
                xml_node<>* nd = nd_opt.get(); // Extract the raw pointer
                long long ref = stoll(std::string(nd->first_attribute("ref")->value()));
                nodeRefs.push_back(ref);
            }

            Way w;
            w.id = stoll(std::string(node->first_attribute("id")->value()));
            // Create way/edges and add way to graph
            for (size_t i = 1; i < nodeRefs.size(); ++i) {
                Edge edge;
                edge.from = nodeRefs[i - 1];
                edge.to = nodeRefs[i];

                Edge reverseEdge;
                reverseEdge.from = nodeRefs[i];
                reverseEdge.to = nodeRefs[i - 1];

                w.edges.push_back(edge);
                graph.edges.push_back(edge);
                graph.edges.push_back(reverseEdge);
            }
            graph.ways[w.id] = w;
        }
    }
}

void ParseMap::createAdj(Graph& graph) {
    // Iterate over edges
    for (const auto& edge : graph.edges) {
        // Get endpoint nodes and calculate weight
        const Node& from = graph.nodes[edge.from];
        const Node& to = graph.nodes[edge.to];
        double weight = getDistance(from, to);

        // Create adj list entry
        graph.adjList[edge.from].push_back({ edge.to,weight });
    }
}

double ParseMap::getDistance(const Node& from, const Node& to) {
    double dx = to.lon - from.lon;
    double dy = to.lat - from.lat;
    return sqrt(dx * dx + dy * dy);
}

