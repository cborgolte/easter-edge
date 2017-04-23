#include <vector>
#include <string>
#include <iostream>
#include <shared_mutex>

#include "boost/graph/adjacency_list.hpp"
#include "boost/any.hpp"


using namespace boost;

typedef std::map<std::string, boost::any> prop_type;

typedef adjacency_list<setS, vecS, bidirectionalS, prop_type, prop_type, prop_type> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;


class State {

	private:

	std::shared_timed_mutex mutexNodes; // protects the nodes vector
	std::shared_timed_mutex mutexGraph; // protects the graph

    std::map<std::string, Graph> graphs;
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

	auto getEdges(std::string graphName) {
		std::shared_lock<std::shared_timed_mutex> lock(mutexGraph);
        const Graph& graph = graphs[graphName];
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
	auto addEdge(const std::string& graphName, 
            const size_t& inNode,
            const size_t& outNode,
            const prop_type& edgeProps) {
		std::lock_guard<std::shared_timed_mutex> guard(mutexGraph);
        Graph& graph = graphs[graphName];
		auto res = add_edge(inNode, outNode, edgeProps, graph);
		return res;
	}

	public:
	auto addEdge(const std::string& graphName,
            const std::string& src,
            const prop_type& srcProps,
            const std::string& target,
            const prop_type& targetProps,
            const prop_type& edgeProps) {

		std::vector<std::string> nodesCreated;

		auto srcNode = getOrInsertNode(src);
		if (srcNode.first) {
			nodesCreated.push_back(src);
		}

		auto targetNode = getOrInsertNode(target);
		if (targetNode.first) {
			nodesCreated.push_back(target);
		}

		return addEdge(graphName, srcNode.second, targetNode.second, edgeProps);
	}
};
