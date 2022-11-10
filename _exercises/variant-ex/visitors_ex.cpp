#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;

// variable template
template <typename T>
constexpr T pi = 3.141592653589793238;

namespace Explain
{
    // variable template

    template <typename T>
    constexpr bool is_integral_v = std::is_integral<T>::value;
}

static_assert(std::is_integral<int>::value);

struct Circle
{
    int radius;

    void draw() const
    {
        std::cout << "Drawing Circle with r: " << radius << "\n";
    }
};

struct Rectangle
{
    int width, height;

    void draw() const
    {
        std::cout << "Drawing Rectangle with w: " << width << " & h: " << height << "\n";
    }
};

struct Square
{
    int size;

    void draw() const
    {
        std::cout << "Drawing Square with size: " << size << "\n";
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

TEST_CASE("visit a shape variant and calculate area")
{
    using Shape = variant<Circle, Rectangle, Square>;

    vector<Shape> shapes = {Circle{1}, Square{10}, Rectangle{10, 1}};

    double total_area{};

    // TODO
    SECTION("return value")
    {
        auto area_calculator = overload{
            [](const Circle& c) -> double
            { return c.radius * c.radius * pi<decltype(total_area)>; },
            [](const Rectangle& r) -> double
            { return r.width * r.height; },
            [](const Square& s) -> double
            { return s.size * s.size; }};

        for (const auto& shape : shapes)
        {
            total_area += std::visit(area_calculator, shape);
        }

        REQUIRE_THAT(pi<double>, Catch::Matchers::WithinRel(3.141, 0.001));
        REQUIRE_THAT(total_area, Catch::Matchers::WithinRel(113.14, 0.01));
    }

    SECTION("captured arg")
    {
        auto area_calculator = overload{
            [&total_area](const Circle& c)
            { total_area += c.radius * c.radius * pi<decltype(total_area)>; },
            [&total_area](const Rectangle& r)
            { total_area += r.width * r.height; },
            [&total_area](const Square& s)
            { total_area += s.size * s.size; }};
        
        for (const auto& shape : shapes)
        {
            std::visit(area_calculator, shape);
        }

        REQUIRE_THAT(pi<double>, Catch::Matchers::WithinRel(3.141, 0.001));
        REQUIRE_THAT(total_area, Catch::Matchers::WithinRel(113.14, 0.01));
    }

    SECTION("drawing")
    {
        for(const auto& shp : shapes)
        {
            std::visit([](const auto& s) { s.draw(); }, shp);
        }
    }
}


class Shape
{
    using TShape = variant<Circle, Rectangle, Square>;

    TShape shape_;
public:
    template <typename T>
    Shape(T&& shape) : shape_{std::forward<T>(shape)}
    {
    }

    template <typename T>
    Shape& operator=(const T& shape) 
    {
        shape_ = shape;

        return *this;
    }

    void draw() const
    {
        std::visit([](const auto& s) { s.draw(); }, shape_);
    }
};

TEST_CASE("polymorphism with variant")
{
    Shape s1 = Circle{10};
    s1.draw();

    s1 = Rectangle{100, 200};
    s1.draw();
}

