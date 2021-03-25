/**
 * fs - filesystem utilities
 */

#pragma once

#include <dirent.h>

#include <fstream>
#include <vector>
#include <string>

namespace fs {

    // The open input and output files (that need to be closed on error)
    extern std::ifstream ifile;
    extern std::ofstream ofile;

    std::vector<std::string> get_compile_list(std::string dir, std::string prefix);

}
