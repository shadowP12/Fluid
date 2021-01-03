#include "CommonUtil.h"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string gProjectDir(PROJECT_DIR);

std::string getProjDir() {
    return gProjectDir;
}

std::string readProjFileData(const std::string& path) {
    std::istream* stream = &std::cin;
    std::ifstream file;

    std::string fullPath = getProjDir() + path;

    file.open(fullPath, std::ios_base::binary);
    stream = &file;
    if (file.fail()) {
        LOGW("cannot open input file %s \n", fullPath.c_str());
        return std::string("");
    }
    return std::string((std::istreambuf_iterator<char>(*stream)), std::istreambuf_iterator<char>());
}

std::string readFileData(const std::string& path) {
    std::istream* stream = &std::cin;
    std::ifstream file;

    file.open(path, std::ios_base::binary);
    stream = &file;
    if (file.fail()) {
        LOGW("cannot open input file %s \n", path.c_str());
        return std::string("");
    }
    return std::string((std::istreambuf_iterator<char>(*stream)), std::istreambuf_iterator<char>());
}