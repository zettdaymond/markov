#include "lib_entry_point.h"

#include <sstream>

#include "markov.h"
#include "utils.h"


std::string analyse(const std::string& incomeJson)
{
    //Parse incoming json
    rapidjson::Document document;

    if (document.Parse(incomeJson.c_str()).HasParseError()) {
        auto errCode = document.GetParseError();
        auto offset = document.GetErrorOffset();

        auto err = EvalError{ (int)errCode, fmt::format("JSON syntax error at position: {}", offset)};
        auto output = errToJson(err);
        return output;
    }


    //Convert json to vector of strategies
    auto jsonParseResult = buildStrategies(document);
    if(jsonParseResult.is_err()) {
        auto err = jsonParseResult.err();
        auto output = errToJson(err);
        return output;
    }

    auto strats = jsonParseResult.value();
    //Run simulation;
    auto simResult = runSimulation(strats.strategies, strats.steps);
    //Build output json based on result
    auto jsonSimResult = formJsonResult(simResult, strats.strategies);

    //print json;
    auto jsonString = utils::jsonToString(jsonSimResult);

    return jsonString;
}


std::string errToJson(const EvalError& err)
{
    std::stringstream ss;
    ss << "{ \"error\" : " << err.code <<" , \"errStr\" : \"" << err.errStr << "\" }";
    return ss.str();
}
