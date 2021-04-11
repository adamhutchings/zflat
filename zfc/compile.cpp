#include "compile.hpp"

namespace {

std::string random_temp_path() {
    std::string r;
    for (int i = 0; i < 20; ++i) {
        r += ('0' + (char) (rand() % 10));
    }
    return r;
}

void compile(std::string in, std::string out, bool leave_at_c) {
    std::ifstream ifile;
    ifile.open(in);
    if (!ifile)
        ZF_ERROR("Could not open %s for reading", in.c_str());
    std::ofstream tmpfil;
    std::string tmp_path = leave_at_c ? out : random_temp_path();
    if (!leave_at_c) tmp_path += ".c";
    tmpfil.open(tmp_path);
    if (!tmpfil)
        ZF_ERROR("Could not open %s for writing", tmp_path.c_str());
    ProgramNode pnode;
    sym::enter_scope(); // global scope
    pnode.read(ifile);
    reorder(&pnode);
    pnode.write(tmpfil);
    ifile.close();
    tmpfil.close();
    if (!leave_at_c) {
        system((std::string("gcc ") + tmp_path + " -o " + out + " -Wall -O3 -c").c_str());
        remove(tmp_path.c_str());
    }
}

std::string get_default_output(std::string path) {
    // turn src/path/to/file.zf into bin/path/to/file.o
    if (path.find(".zf") != path.size() - 3) {
        fprintf(stderr, "zfc: warning: file extension is not .zf\n");
    }
    return std::string("bin/") + path.substr(4, path.size() - 3) + ".o";
}

}

void compile_unit(std::string path, bool leave_at_c) {

    if (path.find(".zf") != path.size() - 3) {
        fprintf(stderr, "zfc: warning: file extension is not .zf\n");
    }
    // compile foo.zf to foo.o
    compile(path, path.substr(0, path.size() - 3) + (leave_at_c ? ".c" : ".o"), leave_at_c);

}

void compile_default() {
    auto files = fs::get_compile_list("src");
    for (auto file : files) {
        compile(file, get_default_output(file), false);
    }
}
