#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>

#include "types.hpp"


struct JsonParse
{
    std::vector<Strategy> strategies;
    int steps = -1;
};


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


JsonParseResult buildStrategies(rapidjson::Document& doc);


SimulationResult runSimulation(std::vector<Strategy>& strategies, int steps);


rapidjson::Document formJsonResult(std::vector<SimulationStepResult>& results,
                                   std::vector<Strategy>& strategies);


StratResult strategyFromJson(const rapidjson::Value& val);


std::string renderSceneGraph(const Strategy& strat);


float calculateQ(const Strategy &s, int i);


void writeNode(std::stringstream& out, const std::string& from,
               const std::string& to, const std::string& arcName);
