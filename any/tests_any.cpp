#include <algorithm>
#include <any>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Data
{
    int a, b;
};

TEST_CASE("any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 42;
    anything = 3.14;
    anything = "text"s;
    anything = Data{10, 20};
    anything = std::vector{1, 2, 3};

    SECTION("access to any - getting a copy")
    {
        auto vec = std::any_cast<std::vector<int>>(anything);
        REQUIRE(vec == std::vector{1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(anything), std::bad_any_cast);
    }

    SECTION("access to any - getting a original value")
    {
        auto* vec = std::any_cast<std::vector<int>>(&anything);

        if (vec)
            REQUIRE(*vec == std::vector{1, 2, 3});

        REQUIRE(std::any_cast<std::string>(&anything) == nullptr);
    }

    SECTION("type")
    {
        anything = Data{67, 55};

        const std::type_info& type_desc = anything.type();
        std::cout << "anything stores: " << type_desc.name() << "\n";
    }
}

class DynamicMap
{
    std::map<std::string, std::any> dict_;
public:
    template <typename T>
    bool insert(std::string key, T value) 
    {
        return dict_.emplace(std::move(key), std::move(value)).second;
    }

    template <typename T>
    T get(const std::string& key) const
    {

        return std::any_cast<T>(dict_.at(key));
    }
};

TEST_CASE("dynamic map")
{
    DynamicMap dm;

    dm.insert("age", 42);
    dm.insert("name", "Jan"s);

    REQUIRE(dm.get<int>("age") == 42);
    REQUIRE(dm.get<std::string>("name") == "Jan");
    REQUIRE_THROWS_AS(dm.get<int>("name"), std::bad_any_cast);
}