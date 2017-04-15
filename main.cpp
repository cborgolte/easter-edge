
#include <vector>
#include <string>
#include <iostream>

#include "boost/graph/adjacency_list.hpp"

#include "crow.h"

using namespace boost;

typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;

template <typename T>
std::string join(const T& v, const std::string& delim) {
    std::ostringstream s;
    for (const auto& i : v) {
        if (&i != &v[0]) {
            s << delim;
        }
        s << i;
    }
    return s.str();
}


std::pair<bool, size_t> getOrInsertNode(const std::string& nodeName, std::vector<std::string>& nodes) {
    auto index = std::find(nodes.begin(), nodes.end(), nodeName);
    if (index == nodes.end()) {
        nodes.push_back(nodeName);
        // inserted
        return std::make_pair(true, nodes.size());
    }
    // no new node
    return std::make_pair(false, index - nodes.begin());
}


int main()
{
    crow::SimpleApp app;

    Graph graph;
    std::vector<std::string> nodes;

    CROW_ROUTE(app, "/nodes")([&nodes](){
         crow::json::wvalue resp;
         resp["nodes"] = nodes;
         return resp;
     });

    CROW_ROUTE(app, "/edges").methods("POST"_method)
        ([&nodes, &graph](const crow::request& req){
         auto doc = crow::json::load(req.body);

         crow::json::wvalue resp;

         if (!doc) {
             resp["status"] = 400;
             return resp;
         }

         std::vector<std::string> nodesCreated;

         std::string in = doc["in"].s();
         std::string out = doc["out"].s();

         auto inNode = getOrInsertNode(in, nodes);
         auto outNode = getOrInsertNode(out, nodes);

         if (inNode.first) {
            nodesCreated.push_back(in);
         }

         if (outNode.first) {
            nodesCreated.push_back(out);
         }

         auto res = add_edge(inNode.second, outNode.second, graph);

         resp["new_nodes"] = nodesCreated;
         //resp["edge"] = res;
         return resp;
    });

    app.port(18080).multithreaded().run();
}
