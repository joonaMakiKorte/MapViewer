#include "Binary.hpp"

void Binary::saveToBinary(const std::string& bin_file_path, const Graph& graph) {
    std::ofstream out_file(bin_file_path, std::ios::binary);
    if (!out_file.is_open()) {
        std::cerr << "Error: Could not open binary file for writing." << std::endl;
        return;
    }

	// Write bounding box
	out_file.write(reinterpret_cast<const char*>(&graph.bbox.min_lat), sizeof(graph.bbox.min_lat));
	out_file.write(reinterpret_cast<const char*>(&graph.bbox.min_lon), sizeof(graph.bbox.min_lon));
	out_file.write(reinterpret_cast<const char*>(&graph.bbox.max_lat), sizeof(graph.bbox.max_lat));
	out_file.write(reinterpret_cast<const char*>(&graph.bbox.max_lon), sizeof(graph.bbox.max_lon));

	// Get edges and nodes from graph
	const std::unordered_map<int64_t, Graph::Node>& nodes = graph.getNodes();
	const std::unordered_map<uint32_t, Graph::Edge>& edges = graph.getEdges();

    // Write counts
    int32_t num_nodes = nodes.size();
    int32_t num_edges = edges.size();
    out_file.write(reinterpret_cast<char*>(&num_nodes), sizeof(num_nodes));
    out_file.write(reinterpret_cast<char*>(&num_edges), sizeof(num_edges));

    // Write nodes
    for (const auto& [node_id, node] : nodes) {
        out_file.write(reinterpret_cast<const char*>(&node_id), sizeof(node_id));
        out_file.write(reinterpret_cast<const char*>(&node.lat), sizeof(node.lat));
        out_file.write(reinterpret_cast<const char*>(&node.lon), sizeof(node.lon));
    }

    // Write edges
    for (const auto& [edge_id, edge] : edges) {
        out_file.write(reinterpret_cast<const char*>(&edge_id), sizeof(edge_id));
        out_file.write(reinterpret_cast<const char*>(&edge.from), sizeof(edge.from));
        out_file.write(reinterpret_cast<const char*>(&edge.to), sizeof(edge.to));
    }

    out_file.close();
    std::cout << "Binary file saved: " << bin_file_path << std::endl;
}

void Binary::loadFromBinary(const std::string& bin_file_path, Graph& graph) {
    std::ifstream in_file(bin_file_path, std::ios::binary);
    if (!in_file.is_open()) {
        std::cerr << "Error: Could not open binary file for reading." << std::endl;
        return;
    }

	// Read bounding box
	in_file.read(reinterpret_cast<char*>(&graph.bbox.min_lat), sizeof(graph.bbox.min_lat));
	in_file.read(reinterpret_cast<char*>(&graph.bbox.min_lon), sizeof(graph.bbox.min_lon));
	in_file.read(reinterpret_cast<char*>(&graph.bbox.max_lat), sizeof(graph.bbox.max_lat));
	in_file.read(reinterpret_cast<char*>(&graph.bbox.max_lon), sizeof(graph.bbox.max_lon));

    // Read counts
    int32_t num_nodes, num_edges;
    in_file.read(reinterpret_cast<char*>(&num_nodes), sizeof(num_nodes));
    in_file.read(reinterpret_cast<char*>(&num_edges), sizeof(num_edges));

    // Read nodes
    for (int i = 0; i < num_nodes; ++i) {
        int64_t id;
        double lat, lon;
        in_file.read(reinterpret_cast<char*>(&id), sizeof(id));
        in_file.read(reinterpret_cast<char*>(&lat), sizeof(lat));
        in_file.read(reinterpret_cast<char*>(&lon), sizeof(lon));
        graph.addNode(id, Graph::Node{ lat, lon });
    }


    // Read edges
    for (int i = 0; i < num_edges; ++i) {
		uint32_t edge_id;
        int64_t from, to;
        in_file.read(reinterpret_cast<char*>(&edge_id), sizeof(edge_id));
        in_file.read(reinterpret_cast<char*>(&from), sizeof(from));
        in_file.read(reinterpret_cast<char*>(&to), sizeof(to));
        graph.addEdge(edge_id, { from, to });
    }

    in_file.close();
    std::cout << "Binary file loaded: " << bin_file_path << std::endl;
}
