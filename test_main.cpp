#include "test_framework_main.h"

void Assert(bool b, const std::string& hint) {
    AssertEqual(b, true, hint);
}

TestRunner Runner;

int main() {
    Runner.RunAllTests();
    if (Runner.HasFails()) {
        std::cerr << Runner.Fails() << " unit tests failed. Terminate" << std::endl;
        return 1;
    }
    std::cerr << "Tests passed." << std::endl;
    return 0;
}