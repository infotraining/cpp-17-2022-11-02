#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <variant>
#include <vector>

using namespace std;

struct Data
{
    int a, b;
};

TEST_CASE("variant")
{
    std::variant<int, double, std::string, Data> var1;

    REQUIRE(std::holds_alternative<int>(var1));
    REQUIRE(std::get<int>(var1) == 0);

    var1 = 42;
    var1 = 3.14;
    var1 = "text"s;
    var1 = Data{42, 665};

    auto& data = std::get<Data>(var1);
    REQUIRE(data.a == 42);
    REQUIRE(data.b == 665);

    REQUIRE_THROWS_AS(std::get<std::string>(var1), std::bad_variant_access);

    Data* ptr_data = std::get_if<Data>(&var1);
    if (ptr_data)
    {
        REQUIRE(ptr_data->a == 42);
    }

    REQUIRE(var1.index() == 3);
}

struct Printer
{
    // std::string_view operator()(int x) const
    // {
    //     std::cout << "Value: " << x << "\n";
    //     return typeid(x).name();
    // }

    // std::string_view operator()(double x) const
    // {
    //     std::cout << "Value: " << x << "\n";
    //     return typeid(x).name();
    // }

    // std::string_view operator()(const std::string& x) const
    // {
    //     std::cout << "Value: " << x << "\n";
    //     return typeid(x).name();
    // }

    template <typename T>
    std::string_view operator()(const T& x) const
    {
        std::cout << "Value: " << x << "\n";
        return typeid(x).name();
    }

    std::string_view operator()(const Data& x) const
    {
        std::cout << "Value: Data{" << x.a << ", " << x.b << "}\n";
        return typeid(x).name();
    }
};

template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

// deduction guide
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

TEST_CASE("visiting variant")
{
    SECTION("with functor Printer")
    {
        std::variant<int, double, std::string, Data> var1;
        var1 = Data{42, 665};
        var1 = "text"s;

        std::string_view type_name = std::visit(Printer{}, var1);
        std::cout << "type: " << type_name << "\n";
    }

    SECTION("with overload")
    {
        std::variant<int, double, std::string, Data> var2 = "text"s;

        auto printer = overload{
            [](const auto& var) { std::cout << "Value: " << var << "\n"; },
            [](const Data& d) { std::cout << "Value: Data{" << d.a << ", " << d.b << "}\n"; }};

        std::visit(printer, var2);
    }
}