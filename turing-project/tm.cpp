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
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

template <>
std::vector<char> splitString(const std::string& str, char delim) {
    std::vector<char> result;
    std::stringstream ss(str);
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
            tm.states = splitString<State>(line.substr(
                line.find('{') + 1, line.find("}") - line.find('{') - 1));
        } else if (isPrefix(line, "#S")) {
            tm.isyms = splitString<InputSymbol>(line.substr(
                line.find('{') + 1, line.find("}") - line.find('{') - 1));
        } else if (isPrefix(line, "#G")) {
            tm.tsyms = splitString<TapSymbol>(line.substr(
                line.find('{') + 1, line.find("}") - line.find('{') - 1));
        } else if (isPrefix(line, "#F")) {
            tm.fstates = splitString<State>(line.substr(
                line.find('{') + 1, line.find("}") - line.find('{') - 1));
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

        // transition function
        else {
            std::vector<std::string> tokens =
                splitString<std::string>(line, ' ');
            assert(tokens.size() == 5);
            State ostate = tokens[0];
            State nstate = tokens[4];

            assert(contains(tm.states, ostate));
            assert(contains(tm.states, nstate));

            std::vector<TapSymbol> osymbols(tokens[1].begin(), tokens[1].end());
            std::vector<TapSymbol> nsymbols(tokens[2].begin(), tokens[2].end());
            std::vector<Direction> dirs(tokens[3].begin(), tokens[3].end());

            // validate symbols
            for (int i = 0; i < tm.N; ++i) {
                TapSymbol osymbol = osymbols[i];
                TapSymbol nsymbol = nsymbols[i];

                assert(contains(tm.tsyms, osymbol) || osymbol == '*');
                assert(contains(tm.tsyms, nsymbol) || osymbol == '*');
            }

            tm.transitions.insert({
                std::make_pair(ostate, osymbols),
                std::make_tuple(nstate, nsymbols, dirs),
            });
        }
    }
    return tm;
}

void TuringMachine::run(std::string input) {
    // initialize tapes and heads
    tapes.resize(N);
    for (int i = 0; i < N; ++i) {
        tapes[i].push_back(blank);
    }
    for (auto c : input) {
        tapes[0].push_back(c);
    }
    for (int i = 0; i < N; ++i) {
        tapes[i].push_back(blank);
    }

    // initialize heads
    heads.resize(N);
    for (int i = 0; i < N; ++i) {
        heads[i] = 1;
    }

    // initialize state
    State state = istate;

    // // run
    // while (!contains(fstates, state)) {
    //     for (int i = 0; i < N; ++i) {
    //         TapSymbol osymbol = tapes[i][heads[i]];
    //         auto it = transitions[i].find(std::make_pair(state, osymbol));
    //         assert(it != transitions[i].end());
    //         State nstate = std::get<0>(it->second);
    //         TapSymbol nsymbol = std::get<1>(it->second);
    //         Direction dir = std::get<2>(it->second);

    //         tapes[i][heads[i]] = nsymbol;
    //         if (dir == ) {
    //             heads[i] -= 1;
    //             if (heads[i] == -1) {
    //                 tapes[i].insert(tapes[i].begin(), blank);
    //                 heads[i] = 0;
    //             }
    //         } else if (dir == Direction::RIGHT) {
    //             heads[i] += 1;
    //             if (heads[i] == tapes[i].size()) {
    //                 tapes[i].push_back(blank);
    //             }
    //         }
    //         state = nstate;
    //     }
    // }

    // // print result
    // for (int i = 0; i < N; ++i) {
    //     for (auto c : tapes[i]) {
    //         std::cout << c;
    //     }
    //     std::cout << std::endl;
    // }
}