#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

typedef std::string State;
typedef char Symbol;
typedef Symbol TapSymbol;
typedef Symbol InputSymbol;

class Direction {
    enum class DirectionType { LEFT, RIGHT, STAY };
    DirectionType direction;
    char ch;

public:
    Direction(DirectionType direction) : direction(direction) {}
    Direction(char dir) {
        if (dir == 'L' || dir == 'l') {
            direction = DirectionType::LEFT;
            ch = 'l';
        } else if (dir == 'R' || dir == 'r') {
            direction = DirectionType::RIGHT;
            ch = 'r';
        } else if (dir == '*') {
            direction = DirectionType::STAY;
            ch = '*';
        } else {
            throw std::invalid_argument("Invalid direction");
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Direction& dir) {
        os << std::string(1, dir.ch);
        return os;
    }
};

typedef std::map<
    std::pair<State, std::vector<TapSymbol>>,
    std::tuple<State, std::vector<TapSymbol>, std::vector<Direction>>>
    Transition;

class TuringMachine {
public:
    // the number of tape
    unsigned int N;
    // transition functions: (ostate, N*osymbol) -> (nstate, N*nsymbol,
    // N*direction)
    Transition transitions;
    // input symbol set
    std::vector<InputSymbol> isyms;
    // tape symbol set
    std::vector<TapSymbol> tsyms;
    // multiple tapes
    std::vector<std::vector<Symbol>> tapes;
    // multiple heads
    std::vector<int> heads;
    // state set
    std::vector<State> states;
    // initial state
    State istate;
    // current state
    State state;
    // blank symbol
    TapSymbol blank;
    // final state set
    std::vector<State> fstates;
    bool isaccpet = false;
    unsigned long long stepcnt = 0;

    static TuringMachine parse(std::string path);

    void run(std::string input);

    friend std::ostream& operator<<(std::ostream& os, const TuringMachine& tm) {
        auto printStringSet = [&os](std::string name,
                                    const std::vector<std::string>& set) {
            os << name << std::string(": {");
            for (auto it = set.begin(); it != set.end(); ++it) {
                os << *it;
                if (it != set.end()) {
                    os << std::string(", ");
                }
            }
            os << std::string("}\n");
        };

        auto printCharSet = [&os](std::string name,
                                  const std::vector<char>& set) {
            os << name << std::string(": {");
            for (auto it = set.begin(); it != set.end(); ++it) {
                os << std::string(1, *it);
                if (it != set.end()) {
                    os << std::string(", ");
                }
            }
            os << std::string("}\n");
        };

        printStringSet("States", tm.states);
        printStringSet("Final states", tm.fstates);
        printCharSet("Input symbols", tm.isyms);
        printCharSet("Tape symbols", tm.tsyms);

        os << std::string("Initial state: ") << tm.istate << std::string("\n");
        os << std::string("Blank symbol: ") << std::string(1, tm.blank)
           << std::string("\n");
        os << std::string("Number of tapes: ") << std::to_string(tm.N)
           << std::string("\n");

        os << std::string("Transition functions:\n");
        for (auto it = tm.transitions.begin(); it != tm.transitions.end();
             ++it) {
            os << std::string("(") << it->first.first << std::string(", ");
            for (auto it2 = it->first.second.begin();
                 it2 != it->first.second.end(); ++it2) {
                os << std::string(1, *it2);
            }
            os << std::string(") -> (") << std::get<0>(it->second)
               << std::string(", ");
            for (auto it2 = std::get<1>(it->second).begin();
                 it2 != std::get<1>(it->second).end(); ++it2) {
                os << std::string(1, *it2);
            }
            os << std::string(", ");
            for (auto it2 = std::get<2>(it->second).begin();
                 it2 != std::get<2>(it->second).end(); ++it2) {
                os << *it2;
            }
            os << std::string(")\n");
        }
        return os;
    }

    void init(std::string input);
    bool isAccept();
    // return true if the machine is still running
    bool step();
    void id();
};