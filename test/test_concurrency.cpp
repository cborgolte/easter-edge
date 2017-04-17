
#include <vector>
#include <string>
#include <iostream>

#include <thread>

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/predicate.hpp"


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

void addEdges(size_t offset, State& state) {
    std::string in("in_");
    std::string out("out_");
    for (size_t i = 0; i < 7000; ++i) {
        std::string num(boost::lexical_cast<std::string>(offset + i));
        state.addEdge(in + num, out + num);
    }
}

void testConcurrency() {
    State state;

    std::thread t1(addEdges, 100000, std::ref(state));
    std::thread t2(addEdges, 200000, std::ref(state));
    t1.join();
    t2.join();

    auto nodes = state.getNodes();
    std::sort(nodes.begin(), nodes.end());
    auto nodesEnd = std::remove_if(nodes.begin(), nodes.end(),
            [](auto val){
                return !(boost::starts_with(val, std::string("in_1")));
            });
    nodes.erase(nodesEnd, nodes.end());
    std::vector<size_t> inNodes(nodes.size());
    std::transform(nodes.begin(), nodes.end(), inNodes.begin(),
            [](auto val){
                return boost::lexical_cast<size_t>(val.substr(3));
            });

    crow::json::wvalue resp;
    resp["nodes"] = inNodes;
    std::cout << crow::json::dump(resp) << std::endl;

    auto itB = inNodes.begin();
    auto itA = itB++;
    while (itB != inNodes.end()) {
        size_t a = *itA;
        size_t b = *itB;
        if (b - a != 1) {
            std::cout  << "Error: " << a << " -> " << b << std::endl;
        }
        ++itA;
        ++itB;
    }
}

int main() {

    testConcurrency();
}

