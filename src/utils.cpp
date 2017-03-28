#include "utils.h"

#include <iostream>

#include <thirdparty/rapidjson/writer.h>

#if defined(GRAPHVIZ_DYNAMIC_RENDERING) || defined(GRAPHVIZ_STATIC_RENDERING)
#include <graphviz/gvc.h>
#else
#include "thirdparty/tiny-process-library/process.hpp"
#endif

namespace utils {

#if defined(GRAPHVIZ_DYNAMIC_RENDERING)
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

#elif defined(GRAPHVIZ_STATIC_RENDERING)

#else

std::string renderToGraph(const std::string& graphDesc)
{
    std::string result;
    Process dot("dot -Tsvg", "", [&](const char *bytes, size_t n) {
        result = std::move( std::string(bytes, n));
    },
    nullptr, true);

    dot.write(graphDesc);
    dot.close_stdin();

    auto errCode = dot.get_exit_status();

    return result;
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
