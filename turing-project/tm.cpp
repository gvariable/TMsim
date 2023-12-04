#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tm.hpp>

template <typename T>
std::vector<T> splitString(const std::string& str, char delim = ',') {
    std::vector<T> result;
    std::stringstream ss(
        str.substr(str.find('{') + 1, str.find("}") - str.find('{') - 1));
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

template <>
std::vector<char> splitString(const std::string& str, char delim) {
    std::vector<char> result;
    std::stringstream ss(
        str.substr(str.find('{') + 1, str.find("}") - str.find('{') - 1));
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item[0]);
    }
    return result;
}

template <typename T>
bool contains(const std::vector<T>& vec, const T& item) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

TuringMachine TuringMachine::parse(std::string path) {
    std::ifstream ifile(path);
    std::string line;

    auto getContentBeforeSemicolon = [](const std::string& str) -> std::string {
        size_t pos = str.find(';');
        return pos != std::string::npos ? str.substr(0, pos) : str;
    };

    auto isPrefix = [](const std::string& str,
                       const std::string& prefix) -> bool {
        return str.rfind(prefix, 0) == 0;
    };

    auto trim = [](std::string& str) -> std::string {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
                      return !std::isspace(ch);
                  }));
        str.erase(std::find_if(str.rbegin(), str.rend(),
                               [](int ch) { return !std::isspace(ch); })
                      .base(),
                  str.end());
        return str;
    };

    TuringMachine tm;

    if (!ifile.is_open()) {
        std::cerr << "Unable to open file: " << path << std::endl;
        exit(1);
    }

    while (std::getline(ifile, line)) {
        // ignore comments and blank lines
        if (line.empty() || line[0] == ';') {
            continue;
        }

        // eliminate inline comments
        line = getContentBeforeSemicolon(line);
        // rstrip whitespaces
        line = trim(line);

        if (isPrefix(line, "#Q")) {
            tm.states = splitString<State>(line);
        } else if (isPrefix(line, "#S")) {
            tm.isyms = splitString<InputSymbol>(line);
        } else if (isPrefix(line, "#G")) {
            tm.tsyms = splitString<TapSymbol>(line);
        } else if (isPrefix(line, "#F")) {
            tm.fstates = splitString<State>(line);
        } else if (isPrefix(line, "#q0")) {
            State istate = line.substr(line.find("=") + 1);
            istate = trim(istate);
            assert(contains(tm.states, istate));
            tm.istate = istate;
        } else if (isPrefix(line, "#B")) {
            std::string s = line.substr(line.find("=") + 1);
            s = trim(s);
            assert(s.size() == 1);
            TapSymbol blank = s[0];
            assert(contains(tm.tsyms, blank));
            tm.blank = blank;
        } else if (isPrefix(line, "#N")) {
            std::string s = line.substr(line.find("=") + 1);
            s = trim(s);
            unsigned int N = std::stoul(s);
            assert(N > 0);
            tm.N = N;
        }
    }
    return tm;
}