#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item)
            {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
} // namespace Cpp98

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
} // namespace BeforeCpp17

namespace SinceCpp17
{
    template <typename... TArgs>
    auto sum(const TArgs&... args)
    {
        return (... + args); // ((((1 + 2) + 3) + 4) + 5);
    }

    template <typename... TArgs>
    auto sum_right(const TArgs&... args)
    {
        return (args + ...); // (1 + ( 2 + (3 + (4 + 5))));
    }

    template <typename... TArgs>
    void print(const TArgs&... args)
    {
        auto with_space = [is_first = true](const auto& item) mutable
        {
            if (!is_first)
                std::cout << " ";
            is_first = false;
            return item;
        };

        (std::cout << ... << with_space(args)) << "\n"; // left binary fold: ((((std::cout << 1) << 42) << 3.14) << "text")
    }

    template <typename... TArgs>
    void print_lines(const TArgs&... args)
    {
        (..., (std::cout << args << "\n"));
    }

    template <typename... TArgs>
    void print_separated(const TArgs&... args)
    {
        (..., (std::cout << args << " ")) << "\n";
    }

    template <typename F, typename... TArgs>
    decltype(auto) apply_function(F&& f, TArgs&&... args)
    {
        return (..., f(std::forward<TArgs>(args)));
    }
}

TEST_CASE("fold expressions for variadic templates")
{
    int result = SinceCpp17::sum(1, 2, 3, 4, 5);
    REQUIRE(result == 15);

    SinceCpp17::print(1, 42, 3.14, "text");

    SinceCpp17::print_lines(1, 42, 3.14, "text");
    
    SinceCpp17::print_separated(1, 42, 3.14, "text");

    SinceCpp17::apply_function([](auto x) { std::cout << "item: " << x << "\n"; }, 1, 3.14, "text");
}
