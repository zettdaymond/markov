#pragma once

#include <string>

#include <fmt/format.h>
#include <rapidjson/document.h>


#ifdef ENABLE_DEBUG_OUTPUT
#define DEBUG(message, args...) \
    fmt::print(#message, ##args);
#endif


namespace utils {
    std::string renderToGraph(const std::string& graphDesc);

    std::string jsonToString(rapidjson::Document& doc);
}
