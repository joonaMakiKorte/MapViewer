#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "rapidxml.hpp"

using namespace rapidxml;

class ParseMap
{
private: 
    struct Node {
        long long id;
        double lat;
        double lon;
    };

    struct Way {
        long long id;
        std::vector<long long> node_refs;
    };

public:
    ParseMap() {};

    void parseOSM() {
        const std::string filePath = "map.osm";

        // Read the file into a string
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << std::endl;
            return;
        }

        std::vector<Node> nodes;
        std::vector<Way> ways;

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
                nodes.push_back(n);
            }

            // Parse Way elements
            else if (nodeName == "way") {
                Way w;
                w.id = stoll(std::string(node->first_attribute("id")->value()));

                // Iterate over nd (node reference) elements
                for (auto nd_opt = node->first_node("nd"); nd_opt; nd_opt = nd_opt->next_sibling("nd")) {
                    xml_node<>* nd = nd_opt.get(); // Extract the raw pointer
                    long long ref = stoll(std::string(nd->first_attribute("ref")->value()));
                    w.node_refs.push_back(ref);
                }

                ways.push_back(w);
            }
        }

        std::cout << "Nodes extracted: " << nodes.size() << " Ways extracted: " << ways.size() << "\n";
    }
};
