#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

std::vector<std::string_view> split_text(std::string_view txt, string_view pattern = ", "sv)
{
    std::vector<std::string_view> res{};
    size_t pos{};
    size_t pos1{};

    while (pos1!= std::string_view::npos) 
    {
        pos1 = txt.find_first_of(pattern, pos);
        res.push_back(txt.substr(pos, pos1 - pos));
        pos = txt.find_first_not_of(pattern, pos1);
    } 
    
    return res;
}

TEST_CASE("split with spaces")
{
    const char* text = "one two three four";

    std::vector<std::string_view> words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}

TEST_CASE("split with commas")
{
    string text = "one,two,three,four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}
