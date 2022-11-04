#include <algorithm>
#include <array>
#include <atomic>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> opt_int;

    REQUIRE(opt_int.has_value() == false);
    REQUIRE(opt_int == std::nullopt);

    if (!opt_int)
    {
        std::cout << "opt_int is empty...\n";
    }

    opt_int = 42;
    REQUIRE(opt_int.has_value());
    REQUIRE(opt_int != nullopt);

    if (opt_int)
    {
        std::cout << "opt_int has value: " << *opt_int << "\n";
    }

    SECTION("unsafe access to optional")
    {
        std::optional<int> opt_value = std::nullopt;

        if (opt_value)
            REQUIRE(*opt_value == 42); 
    }

    SECTION("safe access to optional")
    {
        std::optional<int> opt_value = std::nullopt;

        REQUIRE_THROWS_AS(opt_value.value(), std::bad_optional_access); 

        opt_value = 665;
        REQUIRE(opt_value.value() == 665);
    }
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if(const char* x = std::getenv(n))
        return x;
    else
        return std::nullopt;
}

TEST_CASE("using optional")
{
    std:optional path = maybe_getenv("PATH");

    if (path)
    {
        std::cout << "PATH: " << *path << "\n";
    }

    std::cout << maybe_getenv("PTTH").value_or("ENV NOT FOUND") << "\n";
}