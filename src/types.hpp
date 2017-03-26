#pragma once

#include <vector>
#include "Result.hpp"

struct EvalError
{
    int code;
    std::string errStr;
};


struct Strategy
{
    Strategy() = default;
    Strategy(int _rows, int _cols);

public:
    std::vector<float> probs;
    std::vector<float> revs;
    std::vector<std::string> state_names;

    int rows = 0;
    int cols = 0;
    bool enabled = true;
};
