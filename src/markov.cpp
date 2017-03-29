#include <algorithm>
#include <sstream>
#include <cmath>
#include <thread>
#include <mutex>

#include <thirdparty/rapidjson/writer.h>
#include <thirdparty/fmt/format.h>

#include "markov.h"
#include "utils.h"


JsonParseResult buildStrategies(rapidjson::Document& doc)
{
    int steps = 0;
    std::vector<Strategy> strategies;

    if( not doc.IsObject()) {
        return Err<EvalError>( {10,"JSON root is not an object"} );
    }

    if(not ( doc.HasMember("steps") && doc["steps"].IsInt()) ) {
        return Err<EvalError>( {11,"JSON root have not field 'steps' or field is not an integer"} );
    }

    steps = doc["steps"].GetInt();

    if(not ( doc.HasMember("strategies") && doc["strategies"].IsArray()) ) {
        return Err<EvalError>( {12,"JSON root have not field 'strategies' or this field is not an array"} );
    }

    auto stratArr = doc["strategies"].GetArray();

    for(rapidjson::SizeType i = 0; i < stratArr.Size(); i++) {

        if( not stratArr[i].IsObject()) {
            return Err<EvalError>( {13,"One of root child is not an object"} );
        }

        const rapidjson::Value& strategyJson = stratArr[i].GetObject();

        auto strategyRes = strategyFromJson(strategyJson);
        if(strategyRes.is_err()) {
            return Err<EvalError>( std::move( strategyRes.err() ));
        }

        strategies.push_back(std::move(strategyRes.value()));
    }

    return Ok<JsonParse>( {std::move(strategies), steps} );
}



SimulationResult runSimulation(std::vector<Strategy>& strategies, int steps)
{
    std::vector<SimulationStepResult> result;

    //FIXME: is this check really needs?
    if(strategies.size() < 1 or steps <= 0) {
        return result;
    }


    auto cols = strategies[0].cols;
    auto rows = cols;

    DEBUG("Running sim with {} stages...\n",steps);

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

                //DEBUG("qik = {}\n",q);

                for(int j = 0; j < cols; ++j) {

                    auto pr = prob[i*cols+j];
                    //auto recv = working_vs[j];
                    //qDebug() << "prob=" << pr << " * recv=" << recv;
                    //DEBUG("probs = {} * recv = {}\n",pr,recv);

                    sum += pr  * working_vs[j];
                }

                if(vs[i] < sum) {
                    vs[i] = sum;
                    v_strs[i] = strat;
                }
                //std::cout << "in strategy" << strat << "for column" << i << "selected" << sum << "(" << v_strs[i] << ")" << std::endl;
                DEBUG("In starategy {} for column {} selected {} ({})\n", strat, i, sum, v_strs[i]);
            }
        }

        //std::cout << "stage #" << stage << "vs:" << vs << "choices:" << v_strs << std::endl;
        //DEBUG("Stage {} vs: {} choises: {}", stage, vs, v_strs);

        ++stage;
        if (prev_v_strs == v_strs && stage > 2) {
            std::cout << "more stages not needed" << std::endl;
            break;
        }

        //FIXME: is it possible to move?
        result.emplace_back(vs, v_strs);
    };

    return result;
}



StratResult strategyFromJson(const rapidjson::Value& val)
{
    Strategy s;

    if(not val.HasMember("probabilities")) {
        return Err<EvalError>({100, "One of strategies does not have a field 'probabilities'"});
    }

    if(not val.HasMember("revenues")) {
        return Err<EvalError>({101, "One of strategies does not have a field 'revenues'"});
    }

    if(not val.HasMember("state_names")) {
        return Err<EvalError>({102, "One of strategies does not have a field 'state_names'"});
    }

    if(not val["probabilities"].IsArray()) {
        return Err<EvalError>({103, "In one of strategies field 'probabilities' is not an array"});
    }

    if(not val["revenues"].IsArray()) {
        return Err<EvalError>({104, "In one of strategies field 'revenues' is not an array"});
    }

    if(not val["state_names"].IsArray()) {
        return Err<EvalError>({105, "In one of strategies field 'state_names' is not an array"});
    }


    auto stateNames = val["state_names"].GetArray();

    for(int i = 0; i < stateNames.Size(); i++) {
        if( not stateNames[i].IsString() ) {
            return Err<EvalError>({106, "In one of strategies state name in 'state_names' is not a string"});
        }

        s.state_names.push_back(stateNames[i].GetString());
    }

    auto probsJson = val["probabilities"].GetArray();
    auto revsJson = val["revenues"].GetArray();

    s.rows = probsJson.Size();
    s.cols = s.rows;

    if(s.rows != revsJson.Size()) {
        return Err<EvalError>({107, "In one of strategies array 'probabilities' and array 'revenues' has diffrent sizes"});
    }

    //if(not val.HasMember("id")) {
    //    return Err<EvalError>({101, "One of strategies does not have a field 'id'"});
    //}
    //TODO: check that id is int
    //id = val["id"].GetInt();

    for(rapidjson::SizeType i = 0; i < probsJson.Size(); i++) {
        if(not probsJson[i].IsArray()) {
            return Err<EvalError>({108, "In one of strategies 'probabilities' has row that is not an array"});
        }

        const auto probRowJson = probsJson[i].GetArray();
        if(s.rows != probRowJson.Size()) {
            return Err<EvalError>({109, "In one of strategies 'probabilities' has row with diffrent size than 'probabilities' array itself"});
        }

        for(rapidjson::SizeType j = 0; j < probRowJson.Size(); j++) {
            if(not probRowJson[j].IsNumber()) {
                return Err<EvalError>({110, "In one of strategies 'probabilities' has row with non-number item"});
            }

            auto probValue = probRowJson[j].GetDouble();
            s.probs.push_back(probValue);
        }
    }

    for(rapidjson::SizeType i = 0; i < revsJson.Size(); i++) {
        if(not revsJson[i].IsArray()) {
            return Err<EvalError>({111, "In one of strategies 'revenues' has row that is not an array"});
        }

        const auto revRowJson = revsJson[i].GetArray();
        if(s.rows != revRowJson.Size()) {
            return Err<EvalError>({112, "In one of strategies 'revenues' has row with diffrent size than 'revenues' array itself"});
        }

        for(rapidjson::SizeType j = 0; j < revRowJson.Size(); j++) {
            if(not revRowJson[j].IsNumber()) {
                return Err<EvalError>({113, "In one of strategies 'revenues' has row with non-number item"});
            }

            auto revValue = revRowJson[j].GetDouble();
            s.revs.push_back(revValue);
        }
    }

    return Ok<Strategy>(std::move(s));
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

    //TODO: Candidate for threads
    std::vector<std::thread> threads;
    std::mutex mtx;

    auto& docAllocator = doc.GetAllocator();

#ifdef MULTITHREADED_RENDERING
    for(int i = 0; i < strategies.size(); i++) {
        const auto& s = strategies[i];
        threads.push_back(std::thread([&s, &docAllocator, &mtx, &svgDataJV]{
            auto svg = renderSceneGraph(s);

            rapidjson::Value svgJV;
            svgJV.SetString(svg, docAllocator );

            //critical section
            std::lock_guard<std::mutex> lck (mtx);
            svgDataJV.PushBack(svgJV, docAllocator );
        }));
    }

    for(int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

#else
    for(const auto& s : strategies) {
        auto svg = renderSceneGraph(s);

        rapidjson::Value svgJV;
        svgJV.SetString(svg, docAllocator );

        svgDataJV.PushBack(svgJV, docAllocator );
    }

#endif

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

    DEBUG("{}\n", graphDescription);

    auto svgData = utils::renderToGraph(graphDescription);

    return svgData;
}



void writeNode(std::stringstream& out, const std::string& from,
               const std::string& to, const std::string& arcName)
{
    out << '\"' << from << "\" -> \"" << to
        << "\" [label = \"" << arcName << "\"]\n";
}
