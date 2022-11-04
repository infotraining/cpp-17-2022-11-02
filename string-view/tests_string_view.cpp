#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

void disaster_with_string_view(std::string_view file_name)
{
    FILE* f = fopen(file_name.data(), "rw"); // DISASTER - beware - fopen requires passing null-terminated string
}

TEST_CASE("print_all")
{
    std::vector vec = {1, 2, 3};
    print_all(vec, "vec -----------------------");
}

TEST_CASE("string_view")
{
    std::string_view empty_sv;
    REQUIRE(empty_sv.data() == nullptr);

    std::string empty_str;
    REQUIRE(empty_str.data() != nullptr);

    std::string_view sv1 = "text";
    REQUIRE(sv1 == "text"sv);
    REQUIRE(sv1.size() == 4);
    std::cout << "sv1: " << sv1 << "\n";

    std::string str = "stringified text";
    std::string_view sv2 = str;
    std::cout << "sv2: " << sv2 << "\n";

    std::string token(str.data(), 11) ;
    std::cout << "token: " << token << "\n";

    char letters[3] = {'a', 'b', 'c'};
    std::string_view sv3(letters, 3);
    REQUIRE(sv3 == "abc");
    std::cout << "sv3: " << sv3 << "\n";
    
    //disaster_with_string_view(sv3); // XXX

    std::string_view name = "Jan";
    std::cout << name << "\n";
    
    name = "Adam";
    std::cout << name << "\n";

    std::string str_name(name);
} 

string_view start_from_word(string_view text, string_view word)
{
      return text.substr(text.find(word));
}

std::string get_line()
{
    return "HelloWorld!!!";
}

TEST_CASE("beware of lifetimes")
{
    auto prefix = "World"sv;
    auto text = "HelloWorld!!!";

    auto token = start_from_word(text, prefix);

    // token = start_from_word(get_line(), prefix);  // disaster - returned string_view refers to deallocated memory

    REQUIRE(token == "World!!!");
    std::cout << "token: " << token << "\n";
}

// void print(std::span<int> view)
// {
//     for(const auto& item : view)
//     {
//         std::cout << item << " ";
//     }
//     std::cout << "\n";
// }

// TEST_CASE("C++20")
// {
//     int native_tab[10] = {1, 2, 3, 4, 5};
//     print(std::span{native_tab});

//     std::vector vec = {1, 2, 3, 4, 5};
//     print(std::span{vec.data(), 5});
// }