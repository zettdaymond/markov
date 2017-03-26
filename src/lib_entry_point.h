#pragma once

#include <string>

#include "types.hpp"
#include "Result.hpp"


std::string errToJson(const EvalError& err);

std::string analyse(const std::string& incomeJson);
