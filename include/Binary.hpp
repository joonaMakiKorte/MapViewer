#ifndef BINARY_H
#define BINARY_H

#include "ParseOSM.hpp"

// Binary file format:
// 
// [Header]
// [min_lat: double] [min_lon: double] [max_lat: double] [max_lon: double]
// [num_nodes: int32_t] [num_edges: int32_t]
// 
// [Nodes]
// [node_id: int64_t] [lat: double] [lon: double] * num_nodes
// 
// [Edges]
// [edge_id: uint32_t] [from: int64_t] [to: int64_t] * num_edges


class Binary {
	
public:
	// Serialize data from parseOSM to binary file
	static void saveToBinary(const std::string& bin_file_path, const Graph& graph);

	// Deserialize data from binary file to graph
	static void loadFromBinary(const std::string& bin_file_path, Graph& graph);
};

#endif