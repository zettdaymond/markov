#include <fstream>
#include <iostream>

#include "lib_entry_point.h"


int main(int argc, char *argv[])
{
    if(argc != 2) {
        std::cout << "program /path/to/file.json" << std::endl;
        return -1;
    }

    const auto pathToJson = argv[1];
    std::ifstream ifs(pathToJson);
    std::string income_json( (std::istreambuf_iterator<char>(ifs) ),
                             (std::istreambuf_iterator<char>()    ) );

    std::string outcome_json = analyse(income_json);

    std::cout << outcome_json  << std::endl;

    return 0;
}
