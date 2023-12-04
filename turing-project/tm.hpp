#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

typedef std::string State;
typedef char Symbol;
typedef Symbol TapSymbol;
typedef Symbol InputSymbol;

class Direction {
    enum class DirectionType { LEFT, RIGHT };
    DirectionType direction;

public:
    Direction(DirectionType direction) : direction(direction) {}
    Direction(char direction) {
        if (direction == 'L') {
            this->direction = DirectionType::LEFT;
        } else if (direction == 'R') {
            this->direction = DirectionType::RIGHT;
        }
    }
};

typedef std::map<std::pair<State, TapSymbol>,
                 std::tuple<State, TapSymbol, Direction>>
    Transitions;

class TuringMachine {
public:
    // the number of tape
    unsigned int N;
    // transition functions: (ostate, osymbol) -> (nstate, nsymbol, )
    Transitions transitions;
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
    // blank symbol
    TapSymbol blank;
    // final state set
    std::vector<State> fstates;

    static TuringMachine parse(std::string path);

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

        return os;
    }
};