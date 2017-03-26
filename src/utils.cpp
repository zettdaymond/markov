#include "utils.h"

#include <iostream>

#include <graphviz/gvc.h>

#include <thirdparty/rapidjson/writer.h>

namespace utils {

#ifdef ALLOW_GRAPHVIZ_RENDERING
std::string renderToGraph(const std::string& graphDesc)
{
    GVC_t* gvc = gvContext();

    Agraph_t* G = agmemread(graphDesc.c_str());

    if (G == nullptr) {
        //FIXME: Rewrite to Result monad
        //std::cerr << "Could not create graph from generated string." << std::endl;
    }
    gvLayout (gvc, G, "dot");

    char* result = nullptr;
    unsigned int length;
    gvRenderData (gvc, G, "svg", &result, &length);

    //Do
    //gvFreeRenderData(result);

    gvFreeLayout(gvc, G);
    agclose (G);

    return std::string(result, length);
}

#else

std::string renderToGraph(const std::string& graphDesc)
{
    return "";
}

#endif

std::string jsonToString(rapidjson::Document& doc)
{
    rapidjson::StringBuffer buffer;
    buffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    auto jsonString =  std::string( buffer.GetString() );

    return jsonString;
}


}
