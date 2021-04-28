#include "fs.hpp"

namespace fs {

std::vector<std::string> get_compile_list(std::string dir) {
    std::vector<std::string> ret;
    auto direc = opendir(dir.c_str());
    if (direc == NULL) {
        ZF_ERROR("could not find directory %s", dir.c_str());
    }
    struct dirent* entry;
    while ( (entry = readdir(direc)) != NULL ) {
        if (entry->d_type & DT_DIR) {
            // Make sure to skip . and ..
            if (!(std::string(".") .compare(entry->d_name) &&
                std::string("..").compare(entry->d_name))) {
                ;
            } else {
                auto sub = get_compile_list(dir + entry->d_name + "/");
                for (auto& ent : sub) {
                    ret.push_back(ent);
                }
            }
        } else {
            ret.push_back(dir + "/" + entry->d_name);
        }
    }
    closedir(direc);
    return ret;
}

}
