#include "utils.h"

#include <iostream>

#include <thirdparty/rapidjson/writer.h>


#if defined(GRAPHVIZ_DYNAMIC_RENDERING) || defined(GRAPHVIZ_STATIC_RENDERING)
#	include <mutex>
#	include <graphviz/gvc.h>
#else
#	include "thirdparty/tiny-process-library/process.hpp"
#endif


#ifdef GRAPHVIZ_STATIC_RENDERING

extern gvplugin_library_t gvplugin_core_LTX_library;
extern gvplugin_library_t gvplugin_dot_layout_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
	{ "gvplugin_core_LTX_library", &gvplugin_core_LTX_library},
	{ "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
	{ 0, 0 }
};

#endif


namespace utils {

#if defined(GRAPHVIZ_DYNAMIC_RENDERING) || defined(GRAPHVIZ_STATIC_RENDERING)

static std::mutex mtx;

std::string renderToGraph(const std::string& graphDesc)
{
#ifdef MULTITHREADED_RENDERING
    //critical section for thread safety
    std::lock_guard<std::mutex> lck (mtx);
#endif

#ifdef GRAPHVIZ_STATIC_RENDERING
	GVC_t* gvc = gvContextPlugins(lt_preloaded_symbols, 0);
#else
	GVC_t* gvc = gvContext();
#endif

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

//std::mutex mtx;

std::string renderToGraph(const std::string& graphDesc)
{
    //TODO: add wait_until parameter in the constructor
    std::string result;
    Process dot("dot -Tsvg", "", [&](const char *bytes, size_t n) {
        result.append( bytes, n );
    },
    nullptr, true);

    dot.write(graphDesc);
    dot.close_stdin();

    auto status = dot.get_exit_status();
    DEBUG("Dot process exited with code: {}", status);

    //Check that error happens
    if(status != 0) {
        return "";
    }

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
