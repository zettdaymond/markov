#include "utils.h"

#include <graphviz/gvc.h>
#include <iostream>

namespace utils {

std::string renderToGraph(const std::string& graphDesc)
{
    GVC_t* gvc = gvContext();

    Agraph_t* G = agmemread(graphDesc.c_str());

    if (G == nullptr) {
        std::cerr << "Could not create graph from generated string." << std::endl;
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

}
