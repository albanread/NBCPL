#include "Reducer.h"
#include "Reducers.h"
#include <unordered_map>
#include <functional>



std::unique_ptr<Reducer> createReducer(const std::string& operation_name) {
    // Function-local static to avoid static initialization order issues
    static std::unordered_map<std::string, std::function<std::unique_ptr<Reducer>()>> reducer_factories = {
        {"MIN", []() { return std::make_unique<MinReducer>(); }},
        {"MAX", []() { return std::make_unique<MaxReducer>(); }},
        {"SUM", []() { return std::make_unique<SumReducer>(); }},
        {"PRODUCT", []() { return std::make_unique<ProductReducer>(); }},
        {"BITWISE_AND", []() { return std::make_unique<BitwiseAndReducer>(); }},
        {"BITWISE_OR", []() { return std::make_unique<BitwiseOrReducer>(); }},
        {"PAIRWISE_MIN", []() { return std::make_unique<PairwiseMinReducer>(); }},
        {"PAIRWISE_MAX", []() { return std::make_unique<PairwiseMaxReducer>(); }},
        {"PAIRWISE_ADD", []() { return std::make_unique<PairwiseAddReducer>(); }}
    };
    
    auto it = reducer_factories.find(operation_name);
    if (it != reducer_factories.end()) {
        return it->second();
    }
    return nullptr;
}

bool isReductionOperation(const std::string& operation_name) {
    return createReducer(operation_name) != nullptr;
}