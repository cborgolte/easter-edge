
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

void addEdges(const std::string& graphName, size_t offset, State& state) {
    prop_type prop;
    std::string in("in_");
    std::string out("out_");
    for (size_t i = 0; i < 7000; ++i) {
        std::string num(boost::lexical_cast<std::string>(offset + i));
        state.addEdge(graphName, in + num, prop, out + num, prop, prop);
    }
}

void testConcurrency() {
    State state;

    std::thread t1(addEdges, "G", 100000, std::ref(state));
    std::thread t2(addEdges, "G", 200000, std::ref(state));
    std::thread t3(addEdges, "GG", 300000, std::ref(state));
    std::thread t4(addEdges, "FF", 400000, std::ref(state));
    std::cout << "started threads" << std::endl;
    t1.join();
    std::cout << "joined thread 1" << std::endl;
    t2.join();
    std::cout << "joined thread 2" << std::endl;
    t3.join();
    std::cout << "joined thread 3" << std::endl;
    t4.join();
    std::cout << "joined all threads" << std::endl;

    auto nodes = state.getNodes("G");
    std::cout << "got nodes" << std::endl;
    auto itlow = nodes.lower_bound ("in_1");
    std::cout << "got lower bound " << itlow->first << std::endl;
    auto itup = nodes.upper_bound ("in_1999");
    std::cout << "got upper bound " << itup->first << std::endl;
    std::cout << "nodes size: " << nodes.size() << std::endl;
    nodes.erase(itup, nodes.end());
    nodes.erase(nodes.begin(), itlow);
    std::cout << "erased nodes. size now:  " << nodes.size() << std::endl;
    std::vector<size_t> inNodes(nodes.size());
    std::transform(nodes.begin(), nodes.end(), inNodes.begin(),
            [](auto val){
                //std::cout << "val: " << val.first << std::endl;
                std::string value{val.first.substr(3)};
                //std::cout << " = " << value << std::endl;
                return boost::lexical_cast<size_t>(value);
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

