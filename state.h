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
typedef std::vector<std::string> NodeNameMap;
typedef std::map<std::string, NodeNameMap> NodeCollection;


class State {

	private:

	std::shared_timed_mutex mutexNodes; // protects the nodes vector
	std::shared_timed_mutex mutexGraph; // protects the graph

    std::map<std::string, Graph> graphs;
	NodeCollection nodes;

	public:

	State(const State&) = delete;
	State();

    std::map<std::string, prop_type> getNodes(const std::string& graphName);
    std::vector<std::string> getEdges(std::string graphName);
	std::pair<Graph::edge_descriptor, bool> addEdge(const std::string& graphName,
            const std::string& src,
            const prop_type& srcProps,
            const std::string& target,
            const prop_type& targetProps,
            const prop_type& edgeProps);

	private:
	std::pair<bool, size_t> getOrInsertNode(const std::string& graphName, const std::string& nodeName);
	std::pair<Graph::edge_descriptor, bool> addEdge(const std::string& graphName, 
            const size_t& src,
            const prop_type& srcProps,
            const size_t& target,
            const prop_type& targetProps,
            const prop_type& edgeProps);
};
