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


#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include "lib_entry_point.h"


int main()
{

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

    // Serve it using cppnetlib and the json protocol.
    mhd_json_serve(api, 9999);
}
