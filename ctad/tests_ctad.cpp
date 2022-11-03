#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

void foo(int)
{ }

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg) // universal reference
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    deduce1(x);   // deduce1<int>(int)
    auto ax1 = x; // int
    
    const int cx = 10;
    deduce1(cx);  // deduce1<int>(int)
    auto ax2 = cx; // int

    int& ref_x = x;
    deduce1(ref_x); // deduce1<int>(int)
    auto ax3 = ref_x; // int

    const int& cref_x = cx;
    deduce1(cref_x); // deduce1<int>(int)
    auto ax4 = cref_x; // int
    
    int tab[10];
    deduce1(tab); // deduce1<int*>(int*)
    auto ax5 = tab; // int*
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    deduce2(x);  // deduce2<int>(int&)
    auto& ax1 = x; // int&

    const int cx = 10;
    deduce2(cx); // deduce2<const int>(const int&)
    auto& ax2 = cx; // const int&

    int& ref_x = x;
    deduce2(ref_x); // deduce2<int>(int&)
    auto& ax3 = ref_x; // int&

    const int& cref_x = cx;
    deduce2(cref_x); // deduce2<const int>(const int&)
    auto& ax4 = cref_x; // const int&

    int tab[10];
    deduce2(tab); // deduce2<int[10]>(int(&)[10])
    auto& ax5 = tab; // int(&ax5)[10] = tab
}

TEST_CASE("Template Argument Deduction - case 3")
{
    int x = 10; // lvalue
    deduce3(x);  // deduce<int&>(int&)
    auto&& ax1 = x; // int&

    deduce3(10); // deduce<int>(int&&)
    auto&& ax2 = 10; // int&&
}


//////////////////////////////////////////////////////////////////////
// CTAD

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;
 
    ValuePair(const T1& f, const T2& s) : fst{f}, snd{s}
    {
    }
};

//////////////////////
// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;


template <typename T1, typename T2>
ValuePair<T1, T2> make_value_pair(T1 a, T2 b)
{
    return ValuePair<T1, T2>(a, b);
}

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1{42, 3.14};

    //auto vp2 = make_value_pair<uint8_t>(42, 3.14);

    ValuePair vp3{42, 3.14}; // since C++17 - Class Template Argument Deduction

    ValuePair vp4{42, "forty two"}; // ValuePair<int, const char*>

    ValuePair vp5{42u, "forty two"s}; // ValuePair<unsigned int, std::string>

    // ValuePair<uint8_t> vp6{42, 3.14}; // partial deduction is forbidden
}

// explicit - dygresja :)

// template <typename T1 = int, typename T2 = T1>
// struct Generic 
// {
//     T1 fst;
//     T2 snd;
//     explicit Generic(T1 f = T1{}, T2 s = T2{}) : fst{f}, snd{s} {};
// };

// TEST_CASE("explicit")
// {
//     int x = 42;
//     Generic<uint8_t> g1(x);
// }

TEST_CASE("CTAD - special copy case")
{
    std::vector vec = {1, 2, 3}; // vector<int>

    REQUIRE(vec == std::vector{1, 2, 3});

    std::vector data1{1, 4, 5, 6, 8}; // vector<int>
    std::vector data2{data1}; // special case - CTAD with copy -> vector<int>
    std::vector data3{data1, data1}; // std::vector<std::vector<int>>
}