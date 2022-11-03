#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 543, 235, 42, 534, 654, 3456};

    if (auto pos = std::find(begin(vec), end(vec), 665); pos != end(vec))
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else if (auto pos2 = std::find(begin(vec), end(vec), 42); pos2 != end(vec))
    {
        std::cout << "Item 665 not found!\n";
        assert(pos == end(vec));

        std::cout << "Found: " << *pos2 << "\n";
    }
    else
    {
        std::cout << "Item 42 not found!\n";
        assert(pos2 == end(vec));
        assert(pos == end(vec));
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk{mtx_q_msg};
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::lock_guard lk{mtx_q_msg}; !std::empty(q_msg))
        {
            msg = q_msg.front();
            q_msg.pop();
        }
        else
        {
            q_msg.push("Was empty");
        }
    }
}

TEST_CASE("if with init + structured bindings")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

    if (const auto& [pos, was_inserted] = dict.emplace(2, "dwa"); was_inserted)
    {
        const auto& [key, value] = *pos;
        std::cout << key << " was inserted with value " << value << "\n";
    }
    else
    {
        const auto& [key, value] = *pos;
        std::cout << key << " was already in map with value " << value << "\n";
    }
}

////////////////////////////////////////////////////////
// constexpr if

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

template <typename T>
bool is_power_of_2(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
    else
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

TEST_CASE("constexpr if")
{
    REQUIRE(is_power_of_2(4));
    REQUIRE(is_power_of_2(8));
    REQUIRE(is_power_of_2(32));
    REQUIRE(is_power_of_2(77) == false);

    REQUIRE(is_power_of_2(8.0));
}

namespace [[deprecated]] BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(THead head, TTail... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

namespace SinceCpp17
{
    template <typename THead, typename... TTail>
    void print(THead head, TTail... tail)
    {
        std::cout << head << " ";
        
        if constexpr(sizeof...(tail))
        {
            print(tail...);
        }
        else
        {
            std::cout << "\n";
        }
    }
}

TEST_CASE("constexpr if with variadic templates")
{
    BeforeCpp17::print(1, 2.13, "test"); // print(head = 1, tail = (2.13, "test")) // cout << 1
                            //       -> print(head = 2.13, tail = ("test"))  // cout << 2.13
                            //                -> print(head = "test", tail = ())  // cout << "test"
                            //                         -> print()                     // cout << "\n"
}

template <typename... T1, typename... T2>
void print_with_prefixes(std::tuple<T1...> prefixes, T2... args)
{
    BeforeCpp17::print(args...);
}

TEST_CASE("many tails")
{
    print_with_prefixes(std::tuple{"one", "two", "three"}, 1, 2, 3);
    static_assert(sizeof(int) >= 4);
}
