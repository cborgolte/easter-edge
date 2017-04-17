
#include <vector>
#include <string>
#include <iostream>


#include "crow.h"

#include "state.h"


using namespace boost;


int main() {

	State state;

	crow::SimpleApp app;
	CROW_ROUTE(app, "/nodes")([&state](){
		crow::json::wvalue resp;
		resp["nodes"] = state.getNodes();
		return resp;
	});

	CROW_ROUTE(app, "/edges")
		.methods("POST"_method, "GET"_method)([&state](const crow::request& req){
			crow::json::wvalue resp;
			if (req.method == "POST"_method) {
				auto doc = crow::json::load(req.body);

				if (!doc) {
					resp["status"] = 400;
					resp["msg"] = "Could not parse request body.";
					return resp;
				}

				auto result = state.addEdge(doc["in"].s(), doc["out"].s());

				resp["source"] = result.first.m_source;
				resp["target"] = result.first.m_target;
				resp["created"] = result.second;
			}
			else if (req.method == "GET"_method) {
				resp["edges"] = state.getEdges();
			}
			return resp;
		});

	app.port(18080).multithreaded().run();
	// app.port(18080).run();  // TODO: seems to be faster
}
