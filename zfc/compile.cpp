#include "compile.hpp"

void compile(std::string in, std::string out) {
    std::ifstream ifile;
    ifile.open(in);
    if (ifile.bad())
        ZF_ERROR("Could not open %s for reading", in.c_str());
    std::ofstream ofile;
    ofile.open(out);
    if (ofile.bad())
        ZF_ERROR("Could not open %s for writing", out.c_str());
    ProgramNode pnode;
    pnode.read(ifile);
    pnode.write(ofile);
    ifile.close();
    ofile.close();
}
