#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;

template <typename T>
constexpr T pi = 3.141592653589793238;

struct Circle
{
    int radius;
};

struct Rectangle
{
    int width, height;
};

struct Square
{
    int size;
};

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = variant<Circle, Rectangle, Square>;

    vector<Shape> shapes = {Circle{1}, Square{10}, Rectangle{10, 1}};

    double total_area{};

    // TODO

    REQUIRE_THAT(pi<double>, Catch::Matchers::WithinRel(3.141, 0.001));
    REQUIRE_THAT(total_area, Catch::Matchers::WithinRel(113.14, 0.01));    
}