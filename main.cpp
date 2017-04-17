
#include <vector>
#include <string>
#include <iostream>
#include <shared_mutex>

#include "crow.h"

#include "state.h"


using namespace boost;

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


int main() {
    crow::SimpleApp app;

    State state;

    CROW_ROUTE(app, "/nodes")([&state](){
         crow::json::wvalue resp;
         resp["nodes"] = state.getNodes();
         return resp;
     });

    CROW_ROUTE(app, "/edges")
    .methods("POST"_method, "GET"_method)([&state](const crow::request& req){
        crow::json::wvalue resp;
        if (req.method == "POST"_method) {
            // std::cout << req.body << std::endl;
            auto doc = crow::json::load(req.body);

            if (!doc) {
                resp["status"] = 400;
                return resp;
            }

            std::string in = doc["in"].s();
            std::string out = doc["out"].s();

            auto result = state.addEdge(in, out);

            //resp["new_nodes"] = nodesCreated;
            resp["source"] = result.first.m_source;
            resp["target"] = result.first.m_target;
            resp["created"] = result.second;
        }
        else if (req.method == "GET"_method) {
            resp["edges"] = {"a", "b"};
        }
        return resp;
    });

    app.port(18080).multithreaded().run();
}
