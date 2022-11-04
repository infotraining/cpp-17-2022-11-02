#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <list>
#include <array>

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
 
    // ValuePair(const T1& f, const T2& s) : fst{f}, snd{s}
    // {
    // }

    // ValuePair(T1&& f, T2&& s) : fst{std::move(f)}, snd{std::move(s)}
    // {}

    template <typename U1, typename U2>
    ValuePair(U1&& f, U2&& s) : fst{std::forward<U1>(f)}, snd{std::forward<U2>(s)} // U1&& f and U2&& s - are universal references
    {}
};

//////////////////////
// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*) -> ValuePair<std::string, std::string>;

template <typename T>
ValuePair(T, const char*) -> ValuePair<T, std::string>;

template <typename T>
ValuePair(const char*, T) -> ValuePair<std::string, T>;

///////////////////////
// helper 
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

    ValuePair vp6{"abc", "defg"}; // ValuePair<std::string, std::string> // because of deduction guide line 109

    ValuePair vp7{42, "text"};    
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

/////////////////////////////////////////
// auto vs. decltype

template <typename T>
auto multiply(T a, T b) // -> decltype(a * b)  // since C++14 - auto is enabled for functions
{
    return a * b;
}

auto describe(int x) // auto as deduction mechanism from return
{
    if (x % 2 == 0)
    {
        return "even";
    }

    return "odd";
}

template <typename Map>
decltype(auto) get_value(Map& m, const typename Map::key_type& key) // decltype as deduction mechanism for return
{
    return m.at(key); // deduction using: decltype(m.at(key))
}

TEST_CASE("decltype")
{
    std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };

    using RefMappedValue = decltype(dict[0]);

    auto backup = dict;
    REQUIRE(backup.size() == 2);

    decltype(dict) empty_dict;
    REQUIRE(empty_dict.size() == 0);

    std::string value = "text";
    decltype(dict[0]) item = value; // std::string& item = value;

    std::cout << 2 << " - " << get_value(dict, 2) << "\n";

    get_value(dict, 2) = "dwa";

    std::cout << 2 << " - " << get_value(dict, 2) << "\n";
}


// aggregate
template <typename T>
struct Data
{
    T value;
};

// deduction guide for aggregate
template <typename T>
Data(T) -> Data<T>;

TEST_CASE("CTAD & aggregates")
{
    Data d1{42}; // Data<int>

    Data d2{"text"}; // Data<const char*>
}

int foobar(int x)
{
    return 42 * x;
}

TEST_CASE("std library & CTAD")
{
    SECTION("pair")
    {
        std::pair p1{42, "text"}; // std::pair<int, const char*>
    }

    SECTION("tuple")
    {
        const int cx = 42;
        std::tuple t1{cx, 3.14, "text"}; // std::tuple<int, double, const char*>

        std::pair p{42, "text"};
        std::tuple t2{p};  // std::tuple<int, const char*>
        std::tuple t3 = p; // std::tuple<int, const char*>
    }

    SECTION("optional")
    {
        std::optional opt_int{42}; // std::optional<int>
    }

    SECTION("smart pointers")
    {
        std::unique_ptr<int> uptr{new int(13)}; // CTAD is disabled
        std::shared_ptr<int> sptr{new int(13)}; // CTAD is disabled

        std::unique_ptr uptr_other = std::make_unique<int>(13);
        std::shared_ptr sptr_other = std::move(uptr_other);
        std::weak_ptr wptr = sptr_other;
    }

    SECTION("function")
    {
        std::function f = foobar;
        REQUIRE(f(1) == 42);
    }

    SECTION("containers")
    {
        std::vector vec{1, 2, 3, 4}; // std::vector<int>
        std::list lst{1, 2, 3, 4}; // std::list<int>

        std::vector backup_vec(lst.begin(), lst.end()); // std::vector<int>

        std::array arr{1, 2, 3, 4, 5}; // std::array<int, 5>
    }
}

template <typename T>
struct TypeIdentity
{
    using type = T;
};

template <typename T>
using TypeIdentity_t = typename TypeIdentity<T>::type;

template <typename T>
class UniquePtr
{
    T* ptr_;   
public:
    explicit UniquePtr(TypeIdentity_t<T>* ptr) : ptr_{ptr}
    {}

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& source) noexcept : ptr_{source.ptr_}
    {
        source.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& source)
    {
        if (this != &source)
        {
            delete ptr_;
            ptr_ = source.ptr_;
            source.ptr_ = nullptr;
        }

        return *this;
    }

    ~UniquePtr()
    {
        delete ptr_;
    }

    T* get() const
    {
        return ptr_;
    }

    T& operator*() const
    {
        return *ptr_;
    }

    T* operator->() const
    {
        return ptr_;
    }
};

TEST_CASE("UniquePtr")
{
    UniquePtr<int> uptr{new int(13)};
}