#pragma once

#include <string>

#include "types.hpp"
#include "Result.hpp"


/**
 * @brief Encodes EvalError in JSON format
 * @param err - EvalError instance
 * @return  std::string with encoded error in JSON
 */
std::string errToJson(const EvalError& err);


/**
 * @brief Decode input data, perform all steps of algoritm and decode output data.
 * @param incomeJson - std::string of input data description in JSON  format
 * @return std::string with per step algorithm results or error explanation, all encoded with JSON format.
 */
std::string analyse(const std::string& incomeJson);
