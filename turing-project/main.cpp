#include <iostream>
#include <tm.hpp>
using namespace std;
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input file> <output file>" << endl;
        exit(1);
    }
    auto tm = TuringMachine::parse(argv[1]);

    std::cout << tm << std::endl;
    tm.run(argv[2]);

    return 0;
}