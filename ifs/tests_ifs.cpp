#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("if with initializer")
{
    std::vector vec = {1, 2, 543, 235, 42, 534, 654, 3456};

    if (auto pos = std::find(begin(vec), end(vec), 665); pos != end(vec))
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else if(auto pos2 = std::find(begin(vec), end(vec), 42); pos2 != end(vec) )
    {
        std::cout << "Item 665 not found!\n";
        assert(pos == end(vec));

        std::cout << "Found: " << *pos2 << "\n";
    }
    else
    {
        std::cout << "Item 42 not found!\n";
        assert(pos2 == end(vec));
        assert(pos == end(vec));
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk {mtx_q_msg};
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::lock_guard lk{mtx_q_msg}; !std::empty(q_msg))
        {
            msg = q_msg.front();
            q_msg.pop();
        }
        else
        {
            q_msg.push("Was empty");
        }
    }
}

TEST_CASE("if with init + structured bindings")
{
    std::map<int, std::string> dict = { {1, "one"}, {2, "two"}};

    if (const auto&[pos, was_inserted] = dict.emplace(2, "dwa"); was_inserted)
    {
        const auto& [key, value] = *pos;
        std::cout << key << " was inserted with value " << value << "\n";
    }
    else
    {
        const auto& [key, value] = *pos;
        std::cout << key << " was already in map with value " << value << "\n";
    }
}