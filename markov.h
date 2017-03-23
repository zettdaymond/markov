#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <rapidjson/document.h>


enum GraphColor
{
    BLACK,
    RED,
    GREEN,
    BLUE,
    COUNT
};


struct Strategy
{
    Strategy() = default;
    Strategy(int _rows, int _cols);

    bool fromJson(const rapidjson::Value& val);

public:
    std::vector<float> probs;
    std::vector<float> revs;
    std::vector<std::string> state_names;

    int rows = 0;
    int cols = 0;
    bool enabled = true;
    int id = -1;
};


struct JsonParseResult
{
    bool hasError = false;
    std::string error_str;

    std::vector<Strategy> strategies;
    int steps = -1;
};


struct SimulationStepResult
{
    SimulationStepResult() = default;
    SimulationStepResult(std::vector<float> _vs,std::vector<int> _v_strs) : vs(_vs), v_strs(_v_strs) {}

    std::vector<float> vs;
    std::vector<int>  v_strs;
};


JsonParseResult buildStrategies(rapidjson::Document& doc);

std::vector<SimulationStepResult> runSimulation(std::vector<Strategy>& strategies, int steps);

rapidjson::Document formJsonResult(std::vector<SimulationStepResult>& results,
                                   std::vector<Strategy>& strategies);

std::string renderSceneGraph(const Strategy& strat);

float calculateQ(const Strategy &s, int i);

void writeNode(std::stringstream& out, const std::string& from,
               const std::string& to, const std::string& arcName);
