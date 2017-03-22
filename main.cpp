#include <rapidjson/document.h>
#include <fstream>

#include "markov.h"


int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "program /path/to/file.json" << std::endl;
        return -1;
    }

    const auto pathToJson = argv[1];
    std::ifstream ifs(pathToJson);
    std::string income_json( (std::istreambuf_iterator<char>(ifs) ),
                             (std::istreambuf_iterator<char>()    ) );


    std::string outcome_json = "";

    //Parse incoming json
    rapidjson::Document document;

    if (document.Parse(income_json.c_str()).HasParseError()) {
        auto err = document.GetParseError();
        auto offset = document.GetErrorOffset();

        //TODO: write outcome_json;
        return -1;
    }

    //Convert json to vector of strategies
    auto jsonParseResult = buildStrategies(document);
    if(jsonParseResult.hasError) {
        //TODO: write outcome_json with error;
        std::cout << "Error in jsonParseResult\n";
        return -1;
    }

    //Run simulation;
    auto jsonSimResult = runSimulation(jsonParseResult.strategies, jsonParseResult.step);


    //print json;
    std::cout << jsonSimResult << std::endl;

    return 0;
}
