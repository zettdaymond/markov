#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <fstream>

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

#include "markov.h"


int main()
{

    using namespace sl;
    using namespace s;

    // Create an api
    auto api = http_api(

    _strategies * get_parameters(_strat_json) = [] (auto p)
    {

        return "Hello, worls";
    }

    );

    // Serve it using cppnetlib and the json protocol.
    mhd_json_serve(api, 9999);
}
