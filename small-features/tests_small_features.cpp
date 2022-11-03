#include "gadget.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <bitset>


using namespace std;

TEST_CASE("static inline")
{
    Gadget g1;
    REQUIRE(g1.id() == 1);

    Gadget g2;
    REQUIRE(g2.id() == 2);
}

struct Data1
{
    int a;
    double b;
    std::string c;
};

struct Data2 : Data1
{
    std::vector<int> vec;
};

TEST_CASE("aggregates")
{
    static_assert(std::is_aggregate_v<Data1>);

    Data1 d1{1, 3.14, "test"};

    Data2 d2{{2, 6.28, "base"}, {1, 2, 3}};

    Data2 empty;
}

[[nodiscard]] std::errc open_file(const std::string& name)
{
    return std::errc::bad_file_descriptor;
}

enum Coffee
{
    espresso,
    americano [[deprecated]] = espresso
};

TEST_CASE("attributes")
{
    std::errc error_code = open_file("data.txt");

    [[maybe_unused]] int x;

    switch (error_code)
    {
    case std::errc::bad_file_descriptor:
        std::cout << "Bad file descriptor!\n";
        [[fallthrough]];
    case std::errc::file_too_large:
        std::cout << "File handling!\n";
        break;
    default:
        std::cout << "Other error!\n";
    }
}

///////////////////////////////////////////////////////////////
// Lmabda expressions

struct Object
{
    std::string name;

    auto get_callback()
    {
        return [*this] { std::cout << "Gadget: " << name << "\n"; };
    }
};

TEST_CASE("capturing *this")
{
    std::function<void()> callback;

    {
        Object g{"ipad"};
        callback = g.get_callback();
    }

    callback();
}

TEST_CASE("constexpr lambda")
{
    auto square = [](int x) { return x * x; }; // lambda is implicitly declared

    int native_array[square(8)];
    static_assert(std::size(native_array) == 64);

    std::array<int, square(10)> arr;
    static_assert(std::size(arr) == 100);    
}

template <size_t N>
constexpr auto create_array()
{
    auto square = [](int x) { return x * x; };

    std::array<int, square(N)> data = {};

    for(size_t i = 0; i < square(N); ++i)
        data[i] = square(i);
    
    return data;
}

namespace Cpp20
{
    template <typename Iterator, typename Predicate>
    constexpr Iterator find_if(Iterator first, Iterator last, Predicate pred)
    {
        while(first != last)
        {
            if (pred(*first))
            {
                return first;
            }
            ++first;
        }

        return last;
    }
}

TEST_CASE("constexpr world")
{
    constexpr auto lookup_table_of_squares = create_array<9>();

    constexpr int size_of_buffer = *Cpp20::find_if(lookup_table_of_squares.begin(), lookup_table_of_squares.end(), [](int x) { return x > 1000; });

    constexpr std::array<char, 1024> data{};
}

///////////////////////////////////////////
// enum init

enum class DayOfWeek : uint8_t { mon = 1, tue, wed, thd, fri, sat, sun  };

TEST_CASE("changes for enum")
{
    DayOfWeek today = DayOfWeek::tue;
    uint8_t index = static_cast<uint8_t>(today);

    DayOfWeek tomorrow{5}; // since C++17 - ALLOWED
    REQUIRE(tomorrow == DayOfWeek::fri);
}

TEST_CASE("std::byte")
{
    uint8_t my_old_byte = 64;

    std::byte my_new_byte{64};

    std::byte result = ~(my_new_byte << 2) | std::byte{my_old_byte};

    std::cout << std::to_integer<int>(result) << std::endl;

    std::bitset<32> bs{46};    
}