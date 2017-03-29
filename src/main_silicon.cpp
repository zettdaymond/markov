#include <fstream>
#include <typeinfo>

#include <silicon/api.hh>
#include <silicon/backends/mhd.hh>
#include <iod/symbol.hh>

#ifndef IOD_SYMBOL_hello
#define IOD_SYMBOL_hello
    iod_define_symbol(strategies)
#endif

#ifndef IOD_SYMBOL_name
#define IOD_SYMBOL_name
    iod_define_symbol(strat_json)
#endif


#include <thirdparty/rapidjson/document.h>
#include <thirdparty/rapidjson/writer.h>

#include <thirdparty/fmt/format.h>

#include <thirdparty/cmdline/cmdline.h>

#include "lib_entry_point.h"

struct Config
{
    int port = 9999;
};

Config conf;

int main(int argc, char *argv[])
{
    cmdline::parser a;
    a.add<int>("port", 'p', "Port number. Default is 9999.", false, 9999, cmdline::range(1, 65535));
    a.parse_check(argc, argv);

    conf.port = a.get<int>("port");

    using namespace sl;
    using namespace s;

    // Create an api
    auto api = http_api(

    _strategies * get_parameters(_strat_json = std::string()) = [] (auto p, mhd_request* req, mhd_response* res)
    {
        res->set_header("Access-Control-Allow-Origin", "*");
        res->set_header("Content-Type", "text/javascript");

        std::string outcome_json = analyse(p.strat_json);
        return outcome_json;
    }

    );

    fmt::print("Starting server...\n");
    fmt::print("Start with --help to see help message.\n");
    fmt::print("Listening on port : {}\n", conf.port);

    // Serve it using libmicrohttpd library and the json protocol.
    mhd_json_serve(api, conf.port);

    fmt::print("Shutting Down...\n");
}
