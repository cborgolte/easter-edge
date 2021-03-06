
#include <vector>
#include <string>
#include <iostream>


#include "crow.h"

#include "state.h"


using namespace boost;


any getOrNull(const crow::json::rvalue& val) {
    auto type = val.t();
    
    switch(type){
        case crow::json::type::Number:
            return val.d();
        case crow::json::type::False:
            return false;
        case crow::json::type::True:
            return true;
        case crow::json::type::String:
            return std::string(val.s());
        case crow::json::type::List:
        case crow::json::type::Object:
        default:
            // todo: log error
            std::cerr << "Error: unhandled type: " << std::string(get_type_str(type)) << std::endl;
            break;
    }
    std::cerr << "Error: falling back to default type any() for " << std::string(get_type_str(type)) << std::endl;
    return any();
}


prop_type getOrEmpty(const crow::json::rvalue& obj, const std::string& propName) {
    prop_type result;
    if (obj.has(propName)) {
        auto val = obj[propName];
        std::for_each(val.begin(), val.end(),
                [&result](auto entry) {
                    result[entry.key()] = getOrNull(entry);
                });
    }
    return result;
}

auto setValue = [](auto& rhs, const boost::any& lhs) {
    try {
        rhs = boost::any_cast<std::string>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<int>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<const char*>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<long>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<bool>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<double>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}
    
    try {
        rhs = boost::any_cast<float>(lhs);
        return;
    } catch(const boost::bad_any_cast& exc) {}

    std::cerr << "error, got type: " << lhs.type().name() << std::endl;

    assert(false); // we should never come to this point in code.
    rhs = nullptr;
};


int main() {

	State state;

	crow::SimpleApp app;
	CROW_ROUTE(app, "/graph/<string>/nodes")([&state](std::string graphName){
		crow::json::wvalue resp;
		auto nodes = state.getNodes(graphName);
        size_t cnt{0};
        for (auto nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter, ++cnt) {
            auto& nodeEntry = resp["nodes"][cnt];
            nodeEntry["name"] = nodeIter->first;
            auto properties = nodeIter->second;
            for (auto propertyIter = properties.begin(); propertyIter != properties.end(); ++propertyIter) {
                setValue(nodeEntry["properties"][propertyIter->first], propertyIter->second);
            }
        }
		return resp;
	});

	CROW_ROUTE(app, "/graph/<string>/edges")
		.methods("POST"_method, "GET"_method)([&state](const crow::request& req, std::string graphName){
			crow::json::wvalue resp;
			if (req.method == "POST"_method) {
				auto doc = crow::json::load(req.body);

				if (!doc) {
					resp["status"] = 400;
					resp["msg"] = "Could not parse request body.";
					return resp;
				}

				auto result = state.addEdge(graphName,
                        doc["source"]["name"].s(),
                        getOrEmpty(doc["source"], "properties"),
                        doc["target"]["name"].s(),
                        getOrEmpty(doc["target"], "properties"),
                        getOrEmpty(doc, "properties"));

				resp["source"] = result.first.m_source;
				resp["target"] = result.first.m_target;
				resp["created"] = result.second;
			}
			else if (req.method == "GET"_method) {
				resp["edges"] = state.getEdges(graphName);
			}
			return resp;
		});

	app.port(18080).multithreaded().run();
	// app.port(18080).run();  // TODO: seems to be faster
}
