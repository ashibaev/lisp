#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <functional>
#include <sstream>

template <typename F, typename S>
std::ostream& operator << (std::ostream&, const std::pair<F, S>&);

template <typename F>
std::ostream& operator << (std::ostream&, const std::vector<F>&);

template <typename T>
std::ostream& operator << (std::ostream&, const std::set<T>&);

template <typename F, typename S>
std::ostream& operator << (std::ostream& os, const std::pair<F, S>& p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}


template <class T>
std::ostream& operator << (std::ostream& os, const std::set<T>& s) {
    os << "{";
    bool first = true;
    for (const auto& x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}


template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {}) {
    if (t != u) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << std::endl << "hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

void Assert(bool b, const std::string& hint);


class TestRunner {
private:
    struct TestCase {
        std::function<void()> Test;
        std::string Name;

        TestCase(const std::function<void()>& test, const std::string& name)
            : Test(test)
            , Name(name)
        {
        }
    };

public:
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& testName) {
        try {
            func();
            std::cerr << testName << " OK" << std::endl;
        } catch (std::exception& e) {
            ++FailCount;
            std::cerr << testName << " fail: " << e.what() << std::endl;
            std::cerr << std::endl;
        } catch (...) {
            ++FailCount;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    void AddTest(const std::function<void()>& func, const std::string& testName) {
        TestCases.emplace_back(func, testName);
    }

    bool HasFails() {
        return FailCount > 0;
    }

    uint32_t Fails() {
        return FailCount;
    }

    void RunAllTests() {
        for (const auto& testCase : TestCases) {
            RunTest(testCase.Test, testCase.Name);
        }
    }

private:
    uint32_t FailCount = 0;
    std::vector<TestCase> TestCases;
};



#define TEST_SUITE(NAME)                                        \
namespace TestSuite ## NAME {                                   \
    std::string TestSuiteName = "TestSuite_" #NAME "__";        \
}                                                               \
namespace TestSuite ## NAME



#define TEST(NAME)                                              \
class TestCase ## NAME {                                        \
public:                                                         \
    std::string Name = TestSuiteName + "TestCase_" #NAME;       \
    TestCase ## NAME () {                                       \
        Runner.AddTest(RunTest, Name);                          \
    }                                                           \
    static void RunTest();                                      \
} testCase ## NAME;                                             \
void TestCase ## NAME ::RunTest()



#define ASSERT_THAT_C(A, B) Assert((A), (B));
#define ASSERT_THAT(A) Assert((A), #A " is false");
#define ASSERT_EQUAL(A, B) AssertEqual((A), (B), "[" __FILE__ ", " + std::to_string(__LINE__) + "] " #A " must be equal to " #B)
#define ASSERT_THAT_NOT(A) ASSERT_THAT(!(A))
