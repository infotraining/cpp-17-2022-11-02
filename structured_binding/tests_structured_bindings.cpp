#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct X
{
    int x1;
    double x2;
    std::string x3;
    std::vector<int> x4;
};

using XT = std::tuple<int, double, std::string, std::vector<int>>;

namespace BeforeCpp17
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

tuple<int, int, double> calc_stats(const vector<int>& data)
{
    auto [min_pos, max_pos] = minmax_element(data.begin(), data.end());

    double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

    return tuple(*min_pos, *max_pos, avg);
}

struct Person
{
protected:
    auto tied() const
    {
        return std::tie(fname, lname, age);
    }

public:
    std::string fname;
    std::string lname;
    int age;

    bool operator==(const Person& other) const
    {
        return tied() == other.tied();
    }

    bool operator<=(const Person& other) const
    {
        return tied() <= other.tied();
    }
};

TEST_CASE("tuples")
{
    std::tuple<int, double, std::string> tpl_1; // DEFAULT CONSTRUCTED
    REQUIRE(std::get<0>(tpl_1) == 0);
    REQUIRE(std::get<2>(tpl_1) == "");

    std::tuple<int, double, std::string> tpl_2(42, 3.14, "text");
    REQUIRE(std::get<0>(tpl_2) == 42);
    REQUIRE(std::get<2>(tpl_2) == "text");

    using namespace std::literals;
    auto tpl_3 = std::make_tuple('a', 2.71, "tuple"s); // std::tuple<char, double, std::string>

    SECTION("using tuples for comparison operators")
    {
        Person p1{"Jan", "Kowalski", 42};
        Person p2{"Jan", "Kowalski", 42};

        REQUIRE(p1 == p2);
        REQUIRE(p1 <= p2);
    }
}

TEST_CASE("tie - explain")
{
    SECTION("value tuple")
    {
        int x = 42;
        std::tuple<int, double> tpl{x, 3.14};
        std::get<0>(tpl) = 665;

        REQUIRE(x == 42);
    }

    SECTION("ref tuple")
    {
        int x, y;

        std::tuple<int&, int&> tpl_ref{x, y}; // ref-tuple
        std::get<0>(tpl_ref) = 42;
        REQUIRE(x == 42);

        std::tuple<int, int> values{13, 665};
        tpl_ref = values; // assignment for tuples

        REQUIRE(x == 13);
        REQUIRE(y == 665);

        // (x, y) = (45, 66) // PYTHON
        std::tie(x, y) = std::tuple(45, 99);
    }
}

TEST_CASE("Before C++17")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    int min, max;
    double avg;

    tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == Catch::Approx(665));
    REQUIRE(avg == Catch::Approx(141.333));
}

TEST_CASE("Since C++17 - structured bindings")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    const auto [min, max, avg] = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == Catch::Approx(665));
    REQUIRE(avg == Catch::Approx(141.333));
}

std::array<int, 2> get_position()
{
    return {10, 20};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file()
{
    return ErrorCode{13, "file not found!"};
}

TEST_CASE("structured bindings")
{
    SECTION("native arrays")
    {
        int pos[3] = {10, 20, 30};

        auto [x, y, z] = pos;

        x += 1;

        REQUIRE(x == 11);
        REQUIRE(y == 20);
        REQUIRE(pos[0] == 10);
    }

    SECTION("std::pair")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

        SECTION("Before C++17")
        {
            std::map<int, std::string>::iterator pos;
            bool was_inserted;
            std::tie(pos, was_inserted) = dict.insert(std::make_pair(3, "three"));
        }

        SECTION("Since C++17")
        {
            auto [pos, was_inserted] = dict.insert(std::make_pair(3, "three"));
            REQUIRE(pos->first == 3);
            REQUIRE(was_inserted == true);
        }
    }

    SECTION("std::array")
    {
        std::array<int, 2> pos = {10, 20};

        auto [x, y] = pos;

        REQUIRE(x == 10);
        REQUIRE(y == 20);

        auto [pos_x, pos_y] = get_position();
    }

    SECTION("struct/class")
    {
        auto [error_code, error_message] = open_file();

        REQUIRE(error_code == 13);
        std::cout << "Error: " << error_message << "\n";
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    Timestamp t1;

    auto [hours, minutes, seconds] = t1;

    hours = 15;
    minutes = 41;
    seconds = 30;

    REQUIRE(t1.h == 15);

    SECTION("anonymous entity")
    {
        auto entity = t1;
        auto&& hours = entity.h; // auto&& -> int&
        auto&& minutes = entity.m; // auto&& -> int&
        auto&& seconds = entity.s; // auto&& -> int&
    }
}


TEST_CASE("auto")
{
    int x = 10;
    auto ax1 = x; // copy

    auto& ax2 = x; // no-copy - reference type

    auto&& ax3 = x;  // x is lvalue -> int&
    static_assert(std::is_lvalue_reference_v<decltype(ax3)>);

    auto&& ax4 = 10; // 10 is rvalue -> int&&  
    static_assert(std::is_rvalue_reference_v<decltype(ax4)>);
}

TEST_CASE("use cases")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}, {3, "three"}};

    for(const auto& [key, value] : dict)
    {
        std::cout << key << " - " << value << "\n";
    }
}