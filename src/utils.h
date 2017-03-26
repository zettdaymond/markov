#pragma once

#include <string>

#include <fmt/format.h>
#include <rapidjson/document.h>

#define ENABLE_DEBUG_OUTPUT

#ifdef ENABLE_DEBUG_OUTPUT
#define DEBUG(message, args...) \
    fmt::print(message, ##args);
#else
#define DEBUG(message, args...) ((void)0);
#endif



namespace utils {
    std::string renderToGraph(const std::string& graphDesc);

    std::string jsonToString(rapidjson::Document& doc);
}
