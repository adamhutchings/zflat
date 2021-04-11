/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

#include <compile.hpp>

int main(int argc, char** argv) {
    ++argv, --argc;
    bool one_file = false;
    bool help = true;
    std::string one_file_path = "";
    std::vector<std::string> args;
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
        } else {
            if (one_file) {
                if (one_file_path == "") {
                    one_file_path = str;
                } else {
                    std::cerr << "zfc: file path specified twice\n";
                    exit(-1);
                }
            } else {
                std::cerr << "zfc: arguments unneeded in batch file mode\n";
                exit(-1);
            }
            help = false;
        }
    }
    if (one_file && one_file_path == "") {
        std::cerr << "zfc: expected file after -I flag\n";
        exit(-1);
    }
    if (help) {
        std::cout << "zfc: Compiler for zflat\n"
        "Usage:\n\t"
        "zfc -I [file] (compile one file)\n\t"
        "zfc (compile all files in src directory)\n"
        "Flags:\n\t"
        "--help: see this message\n";
        exit(0);
    }
    if (one_file) {
        compile_unit(one_file_path);
    } else {
        compile_default();
    }
}
