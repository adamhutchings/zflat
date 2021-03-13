/**
 * fs - filesystem utilities
 */

#pragma once

#include <dirent.h>

#include <fstream>
#include <vector>
#include <string>

namespace fs {

    std::vector<std::string> get_compile_list(std::string dir, std::string prefix);

}
