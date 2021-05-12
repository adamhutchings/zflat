#include "compile.hpp"

namespace {

/**
 * Get a random path to store C code in, which is deleted after making an object
 * file. It generates 20 numbers in a row, followed by ".c".
 */
std::string random_temp_path() {
    std::string r;
    for (int i = 0; i < 20; ++i) {
        r += ('0' + (char) (rand() % 10));
    }
    r += ".c";
    return r;
}

/**
 * Strip the file extension off of a file. The argument "ext" is the expected
 * extension - if it is not there, a warning is issued.
 */
std::string strip_extension(std::string path, std::string ext="") {

    auto extdelim = path.find_last_of(".");
    
    if (ext != "" && extdelim != path.size() - (1 + ext.size()) ) {
        ZF_WARN("path %s did not have expected extension %s", path.c_str(), ext.c_str());
    }

    return path.substr(0, extdelim);

}

/**
 * Run the Z-flat core compiler, compiling "in" to path "out".
 */
void zflat_compile(std::string in, std::string out) {

    // Open the input file
    std::ifstream ifile;
    ifile.open(in);
    if (!ifile)
        ZF_ERROR("Could not open %s for reading", in.c_str());
    
    ProgramNode pnode;

    // Run front-end
    pnode.read(ifile);

    // Separate steps here later? (Validation, optimization, etc.)

    // Open the output file
    std::ofstream ofile;
    ofile.open(out);
    if (!ofile) {
        ZF_ERROR("Could not open %s for writing", out.c_str());
    }

    // Write the output C code
    pnode.write(ofile);

    // Close everything
    ifile.close();
    ofile.close();

    // All good!

}

std::string get_c_compiler() {
    // Return the system C compiler. This assumes GCC for now.
    return "gcc";
}

std::string get_c_flags() {
    return "-O3 -Wall -c";
}

/**
 * Run the C compiler.
 */
void cc_compile(std::string in, std::string out) {
    
    std::string cmd = get_c_compiler();
    cmd += " ";
    cmd += in;
    cmd += " -o ";
    cmd += out;
    cmd += " ";
    cmd += get_c_flags();

    system(cmd.c_str());

}

// Remove a temporary intermediate file.
void remove_file(std::string file) {
    std::string cmd = "rm ";
    cmd += file;
    system(cmd.c_str());
}

/**
 * Compile Z-flat to object code.
 */
void zflat_to_object(std::string in, std::string out) {

    std::string tmp = random_temp_path();

    zflat_compile(in, tmp);
    cc_compile(tmp, out);

    remove_file(tmp);
    
}

/**
 * Output the current symbol file from the symbol table.
 */
void write_symbol_file(std::string path) {
    std::ofstream symfil;
    symfil.open(path);
    mod::begin_write(symfil);
    for (sym::Symbol* sym : (*sym::getsymtab())[1]) {
        mod::writesym(symfil, sym);
    }
    mod::end_write(symfil);
    symfil.close();
}

/**
 * Get C output name
 */
std::string get_c_default_output_name(std::string path) {
    return strip_extension(path) + ".c";
}

/**
 * Get object file output name
 */
std::string get_o_default_output_name(std::string path) {
    return strip_extension(path) + ".o";
}

/**
 * Get object file output name, but with bin/obj/ as the prefix.
 */
std::string get_batch_o_default_output_name(std::string path) {
    return std::string("bin/obj/") + strip_extension(path.substr(4)) /* src/ */ + ".o";
}

/**
 * Get symbol file output name.
 */
std::string get_batch_sym_default_output_name(std::string path) {
    return std::string("bin/symbol/") + strip_extension(path.substr(4)) /* src/ */ + ".symbol";
}

}

void compile_unit(std::string path, bool leave_at_c) {
    if (leave_at_c) {
        zflat_compile(path, get_c_default_output_name(path));
    } else {
        zflat_to_object(path, get_o_default_output_name(path));
    }
}

void compile_default() {
    auto files = fs::get_compile_list("src");
    system("mkdir -p bin && mkdir -p bin/symbol && mkdir -p bin/obj");
    for (auto file : files) {
        zflat_to_object(file, get_batch_o_default_output_name(file));
        write_symbol_file(get_batch_sym_default_output_name(file));
    }
}
