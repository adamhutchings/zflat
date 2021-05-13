/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <common/debug.hpp>
#include <compile.hpp>

int main(int argc, char** argv) {
    ++argv, --argc;
    bool one_file = false;
    bool leave_at_c = false;
    bool help = false;
    std::string one_file_path = "";
    std::vector<std::string> args;
    // Default debug level, if no changes
    debug::level = 1;
    while (argc-- > 0) {
        args.push_back(std::string(*(argv++)));
    }
    for (auto str : args) {
        if (str == "-I") {
            if (one_file) {
                std::cout << "zfc: warning: -I flag passed twice\n";
            }
            one_file = true;
        } else if (str == "--help" || str == "-h") {
            help = true;
            break;
        } else if (str == "-c") { 
            leave_at_c = true;
        } else if (str.find("--debug=") == 0) {
            int initial_debug_len = strlen("--debug=");
            if (str.size() == initial_debug_len) {
                ZF_ERROR("Expected value after --debug (ex: --debug=1)");
            }
            str = str.substr(initial_debug_len);
            try {
                debug::level = std::stoi(str);
                if (debug::level < 0 || debug::level > debug::level_max) {
                    ZF_ERROR("Expected valid debug value between 0 and %d (found %s)", debug::level_max, str.c_str());
                }
            } catch (std::invalid_argument a) {
                ZF_ERROR("Expected valid integer debug value");
            }
        } else if (str == "--release") {
            debug::level = 0;
        } else if (str.find("-") == 0) {
            // Other flag option
            ZF_ERROR("unrecognized flag %s", str.c_str());
        } else {
            if (one_file) {
                if (one_file_path == "") {
                    one_file_path = str;
                } else {
                    ZF_ERROR("file path specified twice");
                }
            } else {
                ZF_ERROR("arguments unneeded in batch file mode");
            }
        }
    }
    if (one_file && one_file_path == "") {
        ZF_ERROR("expected file after -I flag");
    }
    if (help) {
        std::cout << "zfc: Compiler for zflat\n"
        "Usage:\n\t"
        "zfc -I [file] (compile one file)\n\t"
        "zfc -I [file] -c (compile to C file)\n\t"
        "zfc (compile all files in src directory)\n"
        "Flags:\n\t"
        "--help: see this message\n";
        exit(0);
    }
    if (one_file) {
        compile_unit(one_file_path, leave_at_c);
    } else {
        compile_default();
    }
}
