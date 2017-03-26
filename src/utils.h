#pragma once

#include <string>

#include <thirdparty/fmt/format.h>
#include <thirdparty/rapidjson/document.h>

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
