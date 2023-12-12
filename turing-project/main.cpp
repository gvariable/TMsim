#include <iostream>
#include <tm.hpp>
using namespace std;

class ArgumentParser {
public:
    bool verbose = false;
    bool help = false;
    string tm;
    string input;

    ArgumentParser(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else if (arg == "-h" || arg == "--help") {
                help = true;
            } else if (arg[0] == '-') {
                cerr << "unknown option: " << arg << endl;
                exit(1);
            } else {
                if (tm.empty()) {
                    tm = arg;
                } else if (input.empty()) {
                    input = arg;
                } else {
                    cerr << "too many arguments" << endl;
                    exit(1);
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    ArgumentParser parser(argc, argv);
    if (parser.help) {
        cout << "usage: " << argv[0]
             << " [-v|--verbose] [-h|--help] <tm> <input>" << endl;
        exit(0);
    }

    auto tm = TuringMachine::parse(parser.tm);

    // std::cout << tm << std::endl;
    tm.run(parser.input, parser.verbose);

    return 0;
}