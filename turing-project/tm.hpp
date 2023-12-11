#pragma once

#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

typedef std::string State;
typedef char Symbol;
typedef Symbol TapSymbol;
typedef Symbol InputSymbol;

class Direction {
public:
    enum class DirectionType { LEFT, RIGHT, STAY };
    DirectionType direction;
    char ch;

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

class TransitionState {
public:
    State ostate;
    std::vector<TapSymbol> osymbols;
    State nstate;
    std::vector<TapSymbol> nsymbols;
    std::vector<Direction> dirs;

    TransitionState(State ostate, std::vector<TapSymbol> osymbols, State nstate,
                    std::vector<TapSymbol> nsymbols,
                    std::vector<Direction> dirs)
        : ostate(ostate),
          osymbols(osymbols),
          nstate(nstate),
          nsymbols(nsymbols),
          dirs(dirs) {}

    friend std::ostream& operator<<(std::ostream& os,
                                    const TransitionState& transition) {
        os << "(" << transition.ostate << ", ";
        for (auto it = transition.osymbols.begin();
             it != transition.osymbols.end(); ++it) {
            os << *it;
        }
        os << ", " << transition.nstate << ", ";
        for (auto it = transition.nsymbols.begin();
             it != transition.nsymbols.end(); ++it) {
            os << *it;
        }
        os << ", ";
        for (auto it = transition.dirs.begin(); it != transition.dirs.end();
             ++it) {
            os << *it;
        }
        os << ")";

        return os;
    }
};

typedef std::map<std::pair<State, std::vector<TapSymbol>>, TransitionState>
    TransitionMap;

class Tape {
public:
    std::vector<TapSymbol> tape;
    unsigned int head;
    TapSymbol blank;

    // move the head to the direction and write the symbol, return true if the
    // tape is changed
    bool move(Direction dir, TapSymbol nsymbol) {
        bool changed = false;
        if (nsymbol != current()) {
            tape[head] = nsymbol;
            changed |= true;
        }

        switch (dir.direction) {
            case Direction::DirectionType::LEFT: {
                changed |= true;
                if (head == 0) {
                    tape.insert(tape.begin(), blank);
                } else {
                    --head;
                }
                break;
            }
            case Direction::DirectionType::RIGHT: {
                changed |= true;
                ++head;
                if (head == tape.size()) {
                    tape.push_back(blank);
                }
                break;
            }
            case Direction::DirectionType::STAY:
                break;
        }
        return changed;
    }

    TapSymbol current() { return tape[head]; }

    friend std::ostream& operator<<(std::ostream& os, const Tape& tape) {
        for (int i = 0; i < tape.tape.size(); ++i) {
            if (tape.tape[i] != tape.blank) {
                os << tape.tape[i];
            }
        }
        return os;
    }
};

class TuringMachine {
public:
    // the number of tape
    unsigned int N;
    // transition map with wildcard : (ostate, N*osymbol) ->
    // (nstate, N*nsymbol, N*direction)
    TransitionMap compactTransitionMap;
    // flat transition map
    TransitionMap transitionMap;
    // input symbol set
    std::vector<InputSymbol> isyms;
    // tape symbol set
    std::vector<TapSymbol> tsyms;
    // multiple tapes
    std::vector<Tape> tapes;
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
            os << name << ": {";
            for (auto it = set.begin(); it != set.end(); ++it) {
                os << *it;
                if (it != set.end() - 1) {
                    os << ", ";
                }
            }
            os << "}" << std::endl;
        };

        auto printCharSet = [&os](std::string name,
                                  const std::vector<char>& set) {
            os << name << ": {";
            for (auto it = set.begin(); it != set.end(); ++it) {
                os << *it;
                if (it != set.end() - 1) {
                    os << ", ";
                }
            }
            os << "}" << std::endl;
        };

        printStringSet("States", tm.states);
        printStringSet("Final states", tm.fstates);
        printCharSet("Input symbols", tm.isyms);
        printCharSet("Tape symbols", tm.tsyms);

        os << "Initial state: " << tm.istate << std::endl;
        os << "Blank symbol: " << tm.blank << std::endl;
        os << "Number of tapes: " << tm.N << std::endl;
        os << "Transition functions:" << std::endl;
        for (auto it = tm.transitionMap.begin(); it != tm.transitionMap.end();
             ++it) {
            os << "(" << it->first.first << ", ";
            for (auto it2 = it->first.second.begin();
                 it2 != it->first.second.end(); ++it2) {
                os << *it2;
            }
            os << ") -> " << it->second << std::endl;
        }

        return os;
    }

    void init(std::string input);
    bool isAccept();
    // return true if the machine is still running
    bool step();
    void id();
};