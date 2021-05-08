#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>

int main() {

    std::cout << "Running test suite ... " << "\n";

    std::string testdir_pass_name = "tests/pass/";
    std::string testdir_fail_name = "tests/fail/";

    struct dirent* entry;
    auto testpass_dir = opendir(testdir_pass_name.c_str());
    auto testfail_dir = opendir(testdir_fail_name.c_str());
    std::vector<std::string> testpass_files, testfail_files;

    if (testpass_dir == NULL || testfail_dir == NULL) {
        std::cerr << "Unable to open both directories: tests/pass/ and tests/fail" << "\n";
        exit(-1);
    }

    while ( (entry = readdir(testpass_dir)) != NULL ) {
        if (entry->d_type & DT_DIR) continue;
        testpass_files.push_back(testdir_pass_name + entry->d_name);
    }

    while ( (entry = readdir(testfail_dir)) != NULL ) {
        if (entry->d_type & DT_DIR) continue;
        testfail_files.push_back(testdir_fail_name + entry->d_name);
    }

    closedir(testpass_dir);
    closedir(testfail_dir);

    // First of all, compile the binary if needed
    auto fp = fopen("./cmake/zfc", "r");
    if (fp == NULL) {
        int compile = system("./scripts/build.sh");
        if (compile) {
            std::cerr << "Could not find binary ./cmake/zfc and compilation failed" << "\n";
            exit(-1);
        }
    } else {
        fclose(fp);
    }

    int passed = 0, failed = 0;

    for (auto name : testpass_files) {
        int result = system( (std::string("./cmake/zfc -I ") + name + " -c").c_str() );
        if (result == 0) {
            ++passed;
        } else {
            // TODO - distinguish fail from compiler breaking
            std::cerr << "Test file " << name << " failed with return code " << result << "." << "\n";
            ++failed;
        }
    }

    for (auto name : testfail_files) {
        int result = system( (std::string("./cmake/zfc -I ") + name + " -c 2>/dev/null").c_str() );
        if (result != 0) {
            // TODO - distinguish fail from compiler breaking
            ++passed;
        } else {
            std::cerr << "Test file " << name << " compiled when it shouldn't have." << "\n";
            ++failed;
        }
    }

    std::cerr << "Test results:" << "\n";

    std::cerr << passed << " passed, " << failed << " failed (" << ((double) passed * 100) / (passed + failed) << "%)" << "\n";

    // Delete all of the output files.
    system("find tests/ -name '*.c' | xargs rm");

    return failed;

}
