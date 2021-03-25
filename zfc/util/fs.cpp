#include "fs.hpp"

namespace fs {

std::ifstream ifile;
std::ofstream ofile;

std::vector<std::string> get_compile_list(std::string dir, std::string prefix=std::string(".")) {
    std::vector<std::string> ret;
    auto direc = opendir(dir.c_str());
    if (direc == NULL) {
        throw "directory not found";
    }
    struct dirent* entry;
    while ( (entry = readdir(direc)) != NULL ) {
        if (entry->d_type & DT_DIR) {
            // Make sure to skip . and ..
            if (std::string(".") .compare(entry->d_name) |
                std::string("..").compare(entry->d_name)) {
                ;
            } else {
                auto sub = get_compile_list(prefix + entry->d_name + "/", std::string(entry->d_name) + "/");
                for (auto& ent : sub) {
                    ret.push_back(ent);
                }
            }
        } else {
            ret.push_back(prefix + "/" + entry->d_name);
        }
    }
    closedir(direc);
    return ret;
}

}
