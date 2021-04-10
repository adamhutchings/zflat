/**
 * fs - filesystem utilities
 */

#pragma once

#include <dirent.h>

#include <fstream>
#include <vector>
#include <string>

#include <util/error.hpp>

namespace fs {

    std::vector<std::string> get_compile_list(std::string dir);

}
