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

    std::vector<float> probs;
    std::vector<float> revs;

    int rows = 0;
    int cols = 0;
    bool enabled = false;
};


struct JsonParseResult
{
    bool hasError = false;
    std::string error_str;

    std::vector<Strategy> strategies;
    int step = -1;
};


JsonParseResult buildStrategies(rapidjson::Document& doc);

std::string runSimulation(std::vector<Strategy>& strategies, int steps);

float calculateQ(const Strategy &s, int i);
