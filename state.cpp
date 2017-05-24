#include <vector>
#include <string>
#include <iostream>
#include <shared_mutex>

#include "boost/graph/adjacency_list.hpp"
#include "boost/any.hpp"

#include "state.h"



State::State(){}

std::map<std::string, prop_type> State::getNodes(const std::string& graphName) {
    std::shared_lock<std::shared_timed_mutex> lock(mutexGraph);

    std::map<std::string, prop_type> result;

    auto graphIter = graphs.find(graphName);
    if (graphIter == graphs.end()) {
        return result;
    }

    const Graph& graph = graphIter->second;
    auto vertexRange{vertices(graph)};
    const NodeNameMap& graphNodes = nodes[graphName];

    std::for_each(
            vertexRange.first,
            vertexRange.second,
            [&graphNodes, &graph, &result](auto vertex){
            const auto& nodeName = graphNodes[vertex];
            result[nodeName] = graph[vertex];
            });

    return result;
}

std::vector<std::string> State::getEdges(std::string graphName) {
    std::shared_lock<std::shared_timed_mutex> lock(mutexGraph);
    const Graph& graph = graphs[graphName];
    auto eds = edges(graph);
    const NodeNameMap& graphNodes = nodes[graphName];
    std::vector<std::string> result;
    for (auto it = eds.first; it != eds.second; ++it) {
        auto source{graphNodes[(*it).m_source]};
        auto target{graphNodes[(*it).m_target]};
        result.push_back(source + std::string(" -> ") + target);
    }
    return result;
}

std::pair<Graph::edge_descriptor, bool> State::addEdge(const std::string& graphName,
        const std::string& src,
        const prop_type& srcProps,
        const std::string& target,
        const prop_type& targetProps,
        const prop_type& edgeProps) {

    std::vector<std::string> nodesCreated;

    auto srcNode = getOrInsertNode(graphName, src);
    if (srcNode.first) {
        nodesCreated.push_back(src);
    }

    auto targetNode = getOrInsertNode(graphName, target);
    if (targetNode.first) {
        nodesCreated.push_back(target);
    }

    auto edgeDesc = addEdge(graphName, srcNode.second, srcProps, targetNode.second,
            targetProps, edgeProps);
    const NodeNameMap& graphNodes = nodes[graphName];
    assert(graphNodes[edgeDesc.first.m_source] == src);
    assert(graphNodes[edgeDesc.first.m_target] == target);
    return edgeDesc;
}

std::pair<bool, size_t> State::getOrInsertNode(const std::string& graphName, const std::string& nodeName) {
    std::lock_guard<std::shared_timed_mutex> guard(mutexNodes);

    NodeNameMap& graphNodes = nodes[graphName];

    auto index = std::find(graphNodes.begin(), graphNodes.end(), nodeName);
    if (index == graphNodes.end()) {
        graphNodes.emplace_back(nodeName);
        // inserted
        return std::make_pair(true, graphNodes.size() - 1);
    }
    // no new node
    return std::make_pair(false, index - graphNodes.begin());
}

std::pair<Graph::edge_descriptor, bool> State::addEdge(const std::string& graphName, 
        const size_t& src,
        const prop_type& srcProps,
        const size_t& target,
        const prop_type& targetProps,
        const prop_type& edgeProps) {
    std::lock_guard<std::shared_timed_mutex> guard(mutexGraph);
    Graph& graph = graphs[graphName];
    std::pair<Graph::edge_descriptor, bool> res = add_edge(src, target, edgeProps, graph);
    graph[res.first.m_source] = srcProps;
    graph[res.first.m_target] = targetProps;
    return res;
}

