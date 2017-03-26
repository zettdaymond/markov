#include "lib_entry_point.h"

#include "markov.h"

#include "utils.h"


std::string analyse(const std::string& incomeJson)
{
    //Parse incoming json
    rapidjson::Document document;

    if (document.Parse(incomeJson.c_str()).HasParseError()) {
        auto err = document.GetParseError();
        auto offset = document.GetErrorOffset();

        //TODO: write outcome_json;
        return "";
    }

    //Convert json to vector of strategies
    auto jsonParseResult = buildStrategies(document);
    if(jsonParseResult.hasError) {
        //TODO: write outcome_json with error;
        std::cout << "Error in jsonParseResult\n";
        return "";
    }

    //Run simulation;
    auto simResult = runSimulation(jsonParseResult.strategies, jsonParseResult.steps);
    //Build output json based on result
    auto jsonSimResult = formJsonResult(simResult, jsonParseResult.strategies);

    //print json;
    auto jsonString = utils::jsonToString(jsonSimResult);

    return jsonString;
}
