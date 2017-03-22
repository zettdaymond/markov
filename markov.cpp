#include <algorithm>
#include <sstream>
#include <cmath>

#include "markov.h"

JsonParseResult buildStrategies(rapidjson::Document& doc)
{
    JsonParseResult res;

    if( not doc.IsArray()) {
        res.hasError = true;
        res.error_str = "Root obj in json is not array";
        return res;
    }

    for(rapidjson::SizeType i = 0; i < doc.Size(); i++) {

        if( not doc[i].IsObject()) {
            res.hasError = true;
            res.error_str = "One of root child is not an object";
            return res;
        }

        const rapidjson::Value& strategyJson = doc[i].GetObject();
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


std::string runSimulation(std::vector<Strategy>& strategies, int steps)
{
    if(strategies.size() < 1) {
        return "";
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

    };

    std::stringstream ss;
    ss << "On step #" << stage << ":<br><br>";
    std::string res_text = "On step #%1:<br><br>";

//    for(int i = 0; i < v_strs.size(); ++i) {
//        res_text.append("<span style=\"color: ");
//        res_text.append(getColorStr((GraphColor)((int)GraphColor::RED+v_strs[i])));
//        res_text.append(";\">");
//        res_text.append(tr("For state \"%1\" optimal strategy is #%2 with revenue of %3").arg(ui->st1_probability->horizontalHeaderItem(i)->text()).arg(v_strs[i]).arg(vs[i]));
//        res_text.append("</span><br><br>");

//    }

//    ui->result_text->setText(res_text);

//    if(strategies[0].enabled) drawStrategyGraph(ui->st1_view,strategies[0],nodeNames, GraphColor::RED);
//    if(strategies[1].enabled) drawStrategyGraph(ui->st2_view,strategies[1],nodeNames, GraphColor::GREEN);
//    if(strategies[2].enabled) drawStrategyGraph(ui->st3_view,strategies[2],nodeNames, GraphColor::BLUE);

    return res_text;
}

bool Strategy::fromJson(const rapidjson::Value& val)
{
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

    auto probsJson = val["probabilities"].GetArray();
    auto revsJson = val["revenues"].GetArray();

    rows = probsJson.Size();
    cols = rows;

    if(rows != revsJson.Size()) {
        return false;
    }

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
