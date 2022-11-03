#include <algorithm>
#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

int get_value()
{
    return 10;
}

TEST_CASE("lvalue-reference vs. rvalue-reference")
{
    SECTION("lvalue-ref")
    {
        int x = 10;
        int& lv_ref = x;
    }

    SECTION("rvalue-ref")
    {
        int&& rv_ref = get_value();
        REQUIRE(rv_ref == 10);
    }
}

std::string full_name(const std::string& fname, const std::string& lname)
{
    return fname + " " + lname;
}

TEST_CASE("binding refs")
{
    std::string name = "Jan";

    SECTION("C++98")
    {
        std::string& ref_name = name;

        const std::string& person_name = full_name(name, "Kowalski");
    }

    SECTION("C++11")
    {
        std::string&& person_name = full_name(name, "Kowalski");
        person_name[0] = 'P';
        REQUIRE(person_name == "Pan Kowalski");

        // std::string&& rvref_name = name; // ERROR
    }
}

class Array
{
    int* items_;
    size_t size_;

public:
    Array(size_t size) : items_{new int[size]}, size_{size}
    {
        std::fill_n(items_, size_, 0);

        std::cout << "Array(size: " << size_ << ", @" << items_ << ")\n";
    }

    Array(std::initializer_list<int> il)
        : items_{new int[il.size()]}
        , size_{il.size()}
    {
        std::copy(il.begin(), il.end(), items_);
        std::cout << "Array({ ";
        for (const auto& item : il)
            std::cout << item << " ";
        std::cout << "}" << ", @" << items_ << ")\n";
    }

    // copy constructor
    Array(const Array& source)
        : items_{new int[source.size()]}
        , size_{source.size()}
    {
        std::cout << "Array(cc: { ";
        for (size_t i = 0; i < source.size(); ++i)
        {
            items_[i] = source[i];
            std::cout << items_[i] << " ";
        }
        std::cout << "}"<< ", @" << items_ << ")\n";
    }

    // copy assignment
    Array& operator=(const Array& source)
    {
        if (this != &source)
        {
            delete[] items_;

            items_ = new int[source.size()];
            size_ = source.size();

            std::cout << "Array::operator=(cc: { ";
            for (size_t i = 0; i < source.size(); ++i)
            {
                items_[i] = source[i];
                std::cout << items_[i] << " ";
            }
            std::cout << "}" << ", @" << items_ << ")\n";
        }

        return *this;
    }

    // move constructor
    Array(Array&& source) noexcept
        : items_{source.items_}
        , size_{source.size_}
    {
        source.items_ = nullptr;
        source.size_ = 0;

        std::cout << "Array(mv: @" << items_ << ")\n";
    }

    // move assignment
    Array& operator=(Array&& source)
    {
        if (this != &source)
        {
            delete[] items_;

            items_ = std::move(source.items_);
            size_ = std::move(source.size_);

            source.items_ = nullptr;
            source.size_ = 0;

            std::cout << "Array::operator=(mv: @" << items_ << ")\n";
        }

        return *this;
    }

    ~Array()
    {
        std::cout << "~Array(@";
        if (items_)
            std::cout << items_;
        else 
            std::cout << "nullptr - state after move";
        std::cout << ")\n";
        
        delete[] items_;
    }

    size_t size() const
    {
        return size_;
    }

    int* data() const
    {
        return items_;
    }

    int& operator[](size_t index)
    {
        return items_[index];
    }

    const int& operator[](size_t index) const
    {
        return items_[index];
    }
};

TEST_CASE("Array - dynamic array")
{
    Array arr_1 = {1, 2, 3, 4};
    REQUIRE(arr_1.size() == 4);
    REQUIRE(arr_1[0] == 1);
    REQUIRE(arr_1[3] == 4);

    Array arr_2 = arr_1; // copy
    REQUIRE(arr_1.data() != arr_2.data());
    REQUIRE(arr_2.size() == 4);
    REQUIRE(arr_2[0] == 1);
    REQUIRE(arr_2[3] == 4);

    int* ptr = arr_1.data();
    Array arr_3 = std::move(arr_1);
    REQUIRE(arr_3.data() == ptr);
    REQUIRE(arr_3.size() == 4);
    REQUIRE(arr_3[0] == 1);
    REQUIRE(arr_3[3] == 4);
}

TEST_CASE("move")
{
    SECTION("primitive types")
    {
        int x = 10;
        int* ptr = &x;

        int y = std::move(x); // cc
        REQUIRE(x == 10);

        int* target = std::move(ptr); // cc
        REQUIRE(ptr == &x);
    }

    SECTION("custom types")
    {
        Array arr_1 = {1, 2, 3};
        Array arr_2 = std::move(arr_1);

        REQUIRE(arr_1.data() == nullptr);
    }
}

namespace Legacy
{
    Array* load_big_data()
    {
        Array* data = new Array(1'000'000);
        for(size_t i = 0; i < data->size(); ++i)
        {
            (*data)[i] = i;
        }

        return data;
    }
}

namespace Modern
{
    Array load_big_data()
    {
        Array data(10);
        for(size_t i = 0; i < data.size(); ++i)
        {
            data[i] = i;
        }

        return data;
    }
}

TEST_CASE("modern cpp with move semantics")
{
    std::cout << "\n-------------------\n";
    Array data_1 = Modern::load_big_data();

    std::vector<Array> dataset;
    dataset.push_back(std::move(data_1));
    data_1 = Modern::load_big_data();
    dataset.push_back(std::move(data_1));
    dataset.push_back(Modern::load_big_data());
    dataset.push_back(Array{1, 2, 3, 4, 5});
}

