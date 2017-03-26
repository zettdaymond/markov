#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>

#include "types.hpp"

/**
 * @brief The JsonParse struct contains parsed strategies and count of algorithm steps to evaluate
 */
struct JsonParse
{
    std::vector<Strategy> strategies;
    int steps = -1;
};


//TODO: write normal explanation
/**
 * @brief The SimulationStepResult struct contains per algoritnm step simulated data
 */
struct SimulationStepResult
{
    SimulationStepResult() = default;
    SimulationStepResult(const std::vector<float>& _vs, const std::vector<int> & _v_strs) : vs(_vs), v_strs(_v_strs) {}

    std::vector<float> vs;
    std::vector<int>  v_strs;
};


using StratResult = Result<Strategy, EvalError>;
using SimulationResult = std::vector<SimulationStepResult>;
using JsonParseResult = Result<JsonParse, EvalError>;


/**
 * @brief Creates array of strategies and determine count of algorithm steps based on JSON deription.
 * @param doc - RapidJSON Document
 * @return Monadic result type Ok<JsonParse> if parse success or EvalError owerwise.
 */
JsonParseResult buildStrategies(rapidjson::Document& doc);

/**
 * @brief Evaluate reccurent algorithm steps.
 * @param strategies - std::vector of strategies Strategy
 * @param steps - count of algorithm steps to evaluate
 * @return std::vector of per step algorithm simualation results
 */
SimulationResult runSimulation(std::vector<Strategy>& strategies, int steps);


/**
 * @brief Creates JSON based algorithm work result representation.
 * @param results - std::vector of per step simulation results SimulationStepResult
 * @param strategies - td::vector of strategies Strategy
 * @return rapidJSON document type
 */
rapidjson::Document formJsonResult(std::vector<SimulationStepResult>& results,
                                   std::vector<Strategy>& strategies);


/**
 * @brief Creates Strategy based on its JSON description
 * @param val - rapidJSON value.
 * @return - Monadic result type Ok<Strategy> if parse success or EvalError owerwise.
 */
StratResult strategyFromJson(const rapidjson::Value& val);


/**
 * @brief Renders strategy graph using GraphViz
 * @param strat - Strategy which graph needs to render
 * @return std::string with svg render data, or empty string if render fail
 */
std::string renderSceneGraph(const Strategy& strat);

//private :

//TODO: write description
float calculateQ(const Strategy &s, int i);


/**
 * @brief create GraphViz strategy's state representation.
 * @param out - stream which to write
 * @param from - what state will be created and FROM which state arc will be rendered
 * @param to - what state will be created and TO which state arc will be rendered
 * @param arcName - name of the created arc
 */
void writeNode(std::stringstream& out, const std::string& from,
               const std::string& to, const std::string& arcName);
