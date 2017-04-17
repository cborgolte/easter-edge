#include <vector>
#include <string>
#include <iostream>
#include <shared_mutex>

#include "boost/graph/adjacency_list.hpp"

using namespace boost;

typedef adjacency_list<setS, vecS, bidirectionalS> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;


class State {

	private:

	std::shared_timed_mutex mutexNodes; // protects the nodes vector
	std::shared_timed_mutex mutexGraph; // protects the graph
	Graph graph;
	std::vector<std::string> nodes;

	public:

	State(const State&) = delete;
	State(){}

	/**
	 * returns a snapshot view of the existing nodes
	 */
	auto getNodes() {
		std::shared_lock<std::shared_timed_mutex> lock(mutexNodes);
		return std::vector<std::string>(nodes);
	}

	auto getEdges() {
		std::shared_lock<std::shared_timed_mutex> lock(mutexGraph);
		auto eds = edges(graph);
		//std::vector<std::map<std::string, std::string>> edgeList;
		//for (auto it = eds.first; it != eds.second; ++it) {
		//	std::map<std::string, std::string> entry;
		//	entry["target"] = nodes[(*it).m_target];
		//	entry["source"] = nodes[(*it).m_source];
		//	edgeList.emplace_back(entry);
		//}
		//return edgeList;
		std::vector<std::string> result;
		for (auto it = eds.first; it != eds.second; ++it) {
			auto source(nodes[(*it).m_source]);
			auto target(nodes[(*it).m_target]);
			result.push_back(source + std::string(" -> ") + target);
		}
		return result;
	}


	private:
	std::pair<bool, size_t> getOrInsertNode(const std::string& nodeName) {
		std::lock_guard<std::shared_timed_mutex> guard(mutexNodes);
		auto index = std::find(nodes.begin(), nodes.end(), nodeName);
		if (index == nodes.end()) {
			nodes.emplace_back(nodeName);
			// inserted
			return std::make_pair(true, nodes.size() - 1);
		}
		// no new node
		return std::make_pair(false, index - nodes.begin());
	}

	private:
	auto addEdge(const size_t& inNode, const size_t& outNode) {
		std::lock_guard<std::shared_timed_mutex> guard(mutexGraph);
		auto res = add_edge(inNode, outNode, graph);
		return res;
	}

	public:
	auto addEdge(const std::string& in, const std::string& out) {

		std::vector<std::string> nodesCreated;

		auto inNode = getOrInsertNode(in);
		if (inNode.first) {
			nodesCreated.push_back(in);
		}

		auto outNode = getOrInsertNode(out);
		if (outNode.first) {
			nodesCreated.push_back(out);
		}

		return addEdge(inNode.second, outNode.second);
	}
};
