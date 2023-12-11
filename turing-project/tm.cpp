#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tm.hpp>

/******************************Help functions
 * begin******************************/
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

// flat transition function
void flat(TransitionMap& transitionMap, const TransitionState& transition,
          const std::vector<TapSymbol>& tsyms, TapSymbol blank) {
    auto wildcardIt =
        std::find(transition.osymbols.begin(), transition.osymbols.end(), '*');

    if (wildcardIt != transition.osymbols.end()) {
        size_t pos = wildcardIt - transition.osymbols.begin();
        TransitionState ntransition = transition;

        for (int i = 0; i < tsyms.size(); ++i) {
            // skip blank symbol
            if (tsyms[i] == blank) {
                continue;
            }
            ntransition.osymbols[pos] = tsyms[i];
            // * -> * keep symbol unchanged
            if (ntransition.nsymbols[pos] == '*') {
                ntransition.nsymbols[pos] = tsyms[i];
            }
            flat(transitionMap, transition, tsyms, blank);
        }
    } else {
        transitionMap.insert(
            {std::make_pair(transition.ostate, transition.osymbols),
             transition});
    }
    return;
}
/******************************Help functions end******************************/

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
                assert(contains(tm.tsyms, nsymbol) || nsymbol == '*');

                // unsupport transition
                if (osymbol != '*' && nsymbol == '*') {
                    std::cerr << "Unsupport transition rule: " << line
                              << std::endl;
                    exit(1);
                }
            }

            tm.compactTransitionMap.insert({
                std::make_pair(ostate, osymbols),
                TransitionState(ostate, osymbols, nstate, nsymbols, dirs),
            });
        }
    }

    // get the actual transition map
    for (auto& [_, value] : tm.compactTransitionMap) {
        flat(tm.transitionMap, value, tm.tsyms, tm.blank);
    }

    return tm;
}

void TuringMachine::init(std::string input) {
    // initialize state
    state = istate;

    // initialize tapes
    tapes.clear();
    tapes.resize(N);

    std::vector<InputSymbol> inputSymbols(input.begin(), input.end());
    for (auto& symbol : inputSymbols) {
        if (!contains(isyms, symbol)) {
            std::cerr << "illegal input string" << std::endl;
            exit(1);
        }
    }

    tapes[0].tape.insert(tapes[0].tape.end(), inputSymbols.begin(),
                         inputSymbols.end());
    tapes[0].head = 0;
    tapes[0].blank = blank;

    for (int i = 1; i < N; ++i) {
        tapes[i].tape.push_back(blank);
        tapes[i].head = 0;
        tapes[i].blank = blank;
    }
}

bool TuringMachine::isAccept() { return isaccpet; }

void TuringMachine::id() {
    static int w = ("Index" + std::to_string(N - 1)).size();
    std::cout << std::left << std::setw(w) << "Step"
              << " : " << stepcnt << std::endl;
    std::cout << std::left << std::setw(w) << "State"
              << " : " << state << std::endl;

    std::cout << std::left << std::setw(w) << "Acc"
              << " : " << (isAccept() ? "Yes" : "No") << std::endl;

    for (int i = 0; i < N; ++i) {
        std::stringstream index, tape, head;
        index << std::left << std::setw(w) << "Index" + std::to_string(i)
              << " : ";
        tape << std::left << std::setw(w) << "Tape" + std::to_string(i)
             << " : ";
        head << std::left << std::setw(w) << "Head" + std::to_string(i)
             << " : ";

        int start = 0, size = 0;
        while (tapes[i].tape[start] == tapes[i].blank &&
               start != tapes[i].head) {
            ++start;
        }

        for (int j = start; j < tapes[i].tape.size(); ++j) {
            if (tapes[i].tape[j] == tapes[i].blank && j != tapes[i].head) {
                continue;
            }
            ++size;
        }

        for (int j = start; j < start + size; ++j) {
            int idx = abs((int)tapes[i].origin - j);
            std::string num = std::to_string(idx);
            index << std::left << std::setw(num.size()) << num;
            tape << std::left << std::setw(num.size()) << tapes[i].tape[j];

            if (j != start + size - 1) {
                index << " ";
                tape << " ";
            }
            if (j < tapes[i].head) {
                head << std::left << std::setw(num.size()) << " "
                     << " ";

            } else if (j == tapes[i].head) {
                head << std::left << std::setw(num.size()) << "^";
            }
        }

        std::cout << index.str() << std::endl;
        std::cout << tape.str() << std::endl;
        std::cout << head.str() << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
}

bool TuringMachine::step() {
    bool ishalt = false;

    std::vector<TapSymbol> osymbols;
    for (int i = 0; i < N; ++i) {
        osymbols.push_back(tapes[i].current());
    }

    // halt or not
    if (transitionMap.find({state, osymbols}) == transitionMap.end()) {
        ishalt = true;
    } else {
        auto transitionState = transitionMap.at({state, osymbols});
        // apply transition rule to tape and head
        for (int i = 0; i < N; ++i) {
            tapes[i].move(transitionState.dirs[i], transitionState.nsymbols[i]);
        }
        // update state
        state = transitionState.nstate;

        if (contains(fstates, transitionState.nstate)) {
            isaccpet = true;
        }
        stepcnt++;
    }

    return ishalt;
}

void TuringMachine::run(std::string input) {
    init(input);

    do {
        id();
    } while (!step());

    if (isaccpet) {
        std::cout << "ACCEPTED" << std::endl;
    } else {
        std::cout << "UNACCEPTED" << std::endl;
    }
    std::cout << "Result: " << tapes[0] << std::endl;
    std::cout << "==================== END ====================";
}
