
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


int main()
{
    crow::SimpleApp app;

    Graph graph;
    std::vector<std::string> nodes;
    std::vector<std::pair<std::string, std::string>> edges;

    CROW_ROUTE(app, "/nodes")
        ([&nodes](){
         //std::ostringstream os;
         //os << join(nodes, ", ");
         //return crow::response{os.str()};

         crow::json::wvalue resp;
         resp["nodes"] = nodes;
         return resp;

         });

    CROW_ROUTE(app, "/edges")
        .methods("POST"_method)
        ([&nodes, &edges, &graph](const crow::request& req){
         auto doc = crow::json::load(req.body);

         crow::json::wvalue resp;

         if (!doc) {
         resp["status"] = 400;
         return resp;
         }
         std::vector<std::string> nodesCreated;
         std::string in = doc["in"].s();
         std::string out = doc["out"].s();
         size_t inPos, outPos;
         auto index = std::find(nodes.begin(), nodes.end(), in);
         if (index == nodes.end()) {
             nodes.push_back(in);
             nodesCreated.push_back(in);
             inPos = nodes.size();
         }
         else {
             inPos = index - nodes.begin();
         }
         index = std::find(nodes.begin(), nodes.end(), out);
         if (index == nodes.end()) {
             nodes.push_back(out);
             nodesCreated.push_back(out);
             outPos = nodes.size();
         }
         else {
             outPos = index - nodes.begin();
         }
         add_edge(inPos, outPos, graph);
         auto edge = std::make_pair(in, out);
         if (std::find(edges.begin(), edges.end(), edge) == edges.end()) {
             edges.push_back(edge);
         }

         resp["new_nodes"] = nodesCreated;
         resp["edge"] = new std::vector<std::string>({edge.first, edge.second});
         return resp;

        });

    app.port(18080).multithreaded().run();
}
