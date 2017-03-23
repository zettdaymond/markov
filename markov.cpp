#include <algorithm>
#include <sstream>
#include <cmath>

#include <rapidjson/writer.h>
#include <fmt/format.h>

#include "markov.h"
#include "utils.h"

JsonParseResult buildStrategies(rapidjson::Document& doc)
{
    JsonParseResult res;
    if( not doc.IsObject()) {
        res.hasError = true;
        res.error_str = "JSON root is not an object";
        return res;
    }

    if(not ( doc.HasMember("steps") && doc["steps"].IsInt()) ) {
        res.hasError = true;
        res.error_str = "JSON root have not field 'steps' or field is not an integer";
        return res;
    }
    res.steps = doc["steps"].GetInt();

    if(not ( doc.HasMember("strategies") && doc["strategies"].IsArray()) ) {
        res.hasError = true;
        res.error_str = "JSON root have not field 'trategies' or this field is not an array";
        return res;
    }
    auto stratArr = doc["strategies"].GetArray();

    for(rapidjson::SizeType i = 0; i < stratArr.Size(); i++) {

        if( not stratArr[i].IsObject()) {
            res.hasError = true;
            res.error_str = "One of root child is not an object";
            return res;
        }

        const rapidjson::Value& strategyJson = stratArr[i].GetObject();
        Strategy strategy;

        if (not strategy.fromJson(strategyJson) ) {
            res.hasError = true;
            res.error_str = "Could not load one of the strategies";
            return res;
        }

        res.strategies.push_back(std::move(strategy));
    }

    return res;
}


std::vector<SimulationStepResult> runSimulation(std::vector<Strategy>& strategies, int steps)
{
    std::vector<SimulationStepResult> result;

    //FIXME: is this check really needs?
    if(strategies.size() < 1 or steps <= 0) {
        return result;
    }


    auto cols = strategies[0].cols;
    auto rows = cols;

    std::cout << "Running sim with" << steps << "stages..." << std::endl;

    std::vector<float> vs(cols);
    std::fill(vs.begin(), vs.end(), 0.0f);

    std::vector<int>  v_strs(cols);
    std::fill(v_strs.begin(), v_strs.end(), -1);

    const int stages = steps;
    int stage = 0;

    while(stage < stages) {
        auto working_vs = vs;
        auto prev_v_strs = v_strs;


        for(int strat = 0; strat < strategies.size(); ++strat) {
            const auto& curr_strat = strategies[strat];
            const auto& prob = curr_strat.probs;

            if(!curr_strat.enabled)
                continue;

            for(int i = 0; i < rows; ++i) {


                auto q = calculateQ(curr_strat, i);
                float sum = q;

                //qDebug() << "qik =" << q;

                for(int j = 0; j < cols; ++j) {

                    auto pr = prob[i*cols+j];
                    //auto recv = working_vs[j];
                    //qDebug() << "prob=" << pr << " * recv=" << recv;


                    sum += pr  * working_vs[j];
                }

                if(vs[i] < sum) {
                    vs[i] = sum;
                    v_strs[i] = strat;
                }
                std::cout << "in strategy" << strat << "for column" << i << "selected" << sum << "(" << v_strs[i] << ")" << std::endl;

            }
        }

        //std::cout << "stage #" << stage << "vs:" << vs << "choices:" << v_strs << std::endl;
        ++stage;
        if (prev_v_strs == v_strs && stage > 2) {
            std::cout << "more stages not needed" << std::endl;
            break;
        }

        result.emplace_back(vs, v_strs);
    };

    return result;
}

bool Strategy::fromJson(const rapidjson::Value& val)
{
    //clear prev
    probs.clear();
    revs.clear();

    if(not val.HasMember("probabilities")) {
        return false;
    }

    if(not val.HasMember("revenues")) {
        return false;
    }

    if(not val["probabilities"].IsArray()) {
        return false;
    }

    if(not val["revenues"].IsArray()) {
        return false;
    }

    if(not val["state_names"].IsArray()) {
        return false;
    }
    auto stateNames = val["state_names"].GetArray();
    for(int i = 0; i < stateNames.Size(); i++) {
        if( not stateNames[i].IsString() ) {
            return false;
        }
        state_names.push_back(stateNames[i].GetString());
    }

    auto probsJson = val["probabilities"].GetArray();
    auto revsJson = val["revenues"].GetArray();

    rows = probsJson.Size();
    cols = rows;

    if(rows != revsJson.Size()) {
        return false;
    }

    if(not val.HasMember("id")) {
        return false;
    }
    id = val["id"].GetInt();

    for(rapidjson::SizeType i = 0; i < probsJson.Size(); i++) {
        if(not probsJson[i].IsArray()) {
            return false;
        }

        const auto probRowJson = probsJson[i].GetArray();
        if(rows != probRowJson.Size()) {
            return false;
        }

        for(rapidjson::SizeType j = 0; j < probRowJson.Size(); j++) {
            if(not probRowJson[j].IsNumber()) {
                return false;
            }

            auto probValue = probRowJson[j].GetDouble();
            probs.push_back(probValue);
        }
    }

    for(rapidjson::SizeType i = 0; i < revsJson.Size(); i++) {
        if(not revsJson[i].IsArray()) {
            return false;
        }

        const auto revRowJson = revsJson[i].GetArray();
        if(rows != revRowJson.Size()) {
            return false;
        }

        for(rapidjson::SizeType j = 0; j < revRowJson.Size(); j++) {
            if(not revRowJson[j].IsNumber()) {
                return false;
            }

            auto revValue = revRowJson[j].GetDouble();
            revs.push_back(revValue);
        }
    }

    enabled = true;

    return true;
}


float calculateQ(const Strategy &s, int i)
{

    int size = std::sqrt(s.probs.size());
    assert(i < size && "invalid size");
    float q = 0.0f;
    for(int j = 0; j < size; ++j) { // Square matrix
        float p = s.probs[i*size + j];
        float r = s.revs[i*size +j];
        q += p*r;
    }
    return q;
}

rapidjson::Document formJsonResult(std::vector<SimulationStepResult>& results, std::vector<Strategy>& strategies)
{
    //TODO: use strategy id, instead of position in array
    rapidjson::Document doc;
    doc.SetObject();

    //Create states for each step
    rapidjson::Value stepsJV;
    stepsJV.SetArray();

    for(int step = 0; step < results.size(); step++) {
        const SimulationStepResult& stepRes = results[step];

        rapidjson::Value stepJV;
        stepJV.SetObject();

        rapidjson::Value stepNumJV(step);
        stepJV.AddMember("step_num", stepNumJV.Move(), doc.GetAllocator());

        rapidjson::Value statesJV;
        statesJV.SetArray();

        for(int state = 0; state < stepRes.v_strs.size(); state++) {
            rapidjson::Value stateNumJV(state);
            rapidjson::Value optimalStratJV(stepRes.v_strs[state]);
            rapidjson::Value revenueJV(stepRes.vs[state]);

            rapidjson::Value stateObjJV;
            stateObjJV.SetObject();

            stateObjJV.AddMember("state_num", stateNumJV, doc.GetAllocator());
            stateObjJV.AddMember("optimal_strategy", optimalStratJV.Move(), doc.GetAllocator());;
            stateObjJV.AddMember("optimal_strategy_revenue", revenueJV.Move(), doc.GetAllocator());

            statesJV.PushBack(stateObjJV.Move(), doc.GetAllocator());
        }

        stepJV.AddMember("states", statesJV.Move(), doc.GetAllocator());

        stepsJV.PushBack(stepJV, doc.GetAllocator());
    }


    doc.AddMember("steps", stepsJV.Move(), doc.GetAllocator());

    //Create SVG-DATA
    rapidjson::Value svgDataJV;
    svgDataJV.SetArray();

    for(auto& s : strategies) {
        auto svg = renderSceneGraph(s);

        rapidjson::Value svgJV;
        svgJV.SetString(svg, doc.GetAllocator());
        svgDataJV.PushBack(svgJV, doc.GetAllocator());
    }

    doc.AddMember("svg_data", svgDataJV.Move(), doc.GetAllocator());

    return doc;
}

std::string renderSceneGraph(const Strategy& strat)
{
    std::stringstream scriptStream;
    scriptStream << "digraph G {  nodesep=1.25 ranksep=1.25\n";
    //TODO: Random color?
    //scriptStream << "node [shape=ellipse, width=0.75, height=0.75, color=\"" << c.name() << "\"]\n";
    //scriptStream << "edge [color=\"" << c.name() << "\"]\n";

    int states = strat.state_names.size();
    auto s = strat;

    for(int i = 0; i < states; i++) {
        for(int j = 0; j < states; j++) {
            auto probability = s.probs[i * states + j];
            if(probability == 0) {
                continue;
            }
            auto revenue = s.revs[i * states + j];

            auto fromStr = s.state_names[i];
            auto toStr = s.state_names[j];

            writeNode(scriptStream,fromStr, toStr, fmt::format("{0}\n {1}$", probability,revenue));
        }
    }

    scriptStream << '}';
    scriptStream.flush();

    std::string graphDescription = scriptStream.str();

    std::cout << graphDescription << std::endl;

    auto svgData = utils::renderToGraph(graphDescription);

    return svgData;
}


void writeNode(std::stringstream& out, const std::string& from,
               const std::string& to, const std::string& arcName)
{
    out << '\"' << from << "\" -> \"" << to
        << "\" [label = \"" << arcName << "\"]\n";
}


std::string doWork(const std::string& incomeJson)
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
    rapidjson::StringBuffer buffer;

    buffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    jsonSimResult.Accept(writer);

    auto jsonString =  std::string( buffer.GetString() );
    return jsonString;
}
