#include <iostream>
#include <assert.h>

#include "../app/utils/tuple_utils.hpp"

using rpc::utils::tuple::is_tuple;
using rpc::utils::tuple::tuple_for_each_invocable;
using rpc::utils::tuple::apply_for_each;

//* Testing is_tuple<>
static_assert(is_tuple<std::tuple<>>);

static_assert(is_tuple<std::tuple<int>>);
static_assert(is_tuple<std::tuple<int&>>);
static_assert(is_tuple<std::tuple<int&>&>);

static_assert(is_tuple<std::tuple<int>>);
static_assert(is_tuple<std::tuple<int>&>);
static_assert(is_tuple<std::tuple<int>&&>);

static_assert(is_tuple<std::tuple<int, char>>);
static_assert(is_tuple<std::tuple<int&, char>&>);
static_assert(is_tuple<std::tuple<int&, char>&>);

static_assert(is_tuple<std::tuple<int&, char&>&&>);
static_assert(is_tuple<std::tuple<int&, char>&&>);

static_assert(is_tuple<std::tuple<std::tuple<int, char>>>);

static_assert(!is_tuple<std::pair<int, char>>);
static_assert(!is_tuple<std::array<int, 5>>);

//* Testing tuple_for_each_invocable<>
static_assert(tuple_for_each_invocable<decltype([](int&) {}), std::tuple<int>&>);
static_assert(!tuple_for_each_invocable<decltype([](int&&) {}), std::tuple<int>&>);

static_assert(tuple_for_each_invocable<decltype([](int&&) {}), std::tuple<int>&&>);
static_assert(!tuple_for_each_invocable<decltype([](int&) {}), std::tuple<int>&&>);

static_assert(tuple_for_each_invocable<decltype([](const int&) {}), const std::tuple<int>&>);
static_assert(!tuple_for_each_invocable<decltype([](int&) {}), const std::tuple<int>&>);

static_assert(!tuple_for_each_invocable<decltype([](int&, const std::string&) {}), std::tuple<int&, const std::string&>&>);
static_assert(!tuple_for_each_invocable<decltype([](int){}), std::pair<int, int>>);

int main()
{
    int counter = 1;
    const auto tuple = std::make_tuple(1, 2, 3, "okay");
    //* Testing apply_for_tuple 
    {
        auto print_tuple = []<typename T>(T&& x)
        {
            std::cout << x << " ";
        };

        apply_for_each(tuple, print_tuple);
        std::cout << std::endl;
        std::cout << counter++ << ".Pass\n" << std::endl;
    }
    {
        auto copy_tuple = tuple;
        auto& lref_tuple = copy_tuple;

        auto lambda = []<typename T>(T&& x)
        {
            static_assert(std::is_lvalue_reference_v<T>);
            if constexpr(std::same_as<int, std::remove_cvref_t<T>>)
                x+=10;
            else if constexpr(std::same_as<const char*, std::remove_cvref_t<T>>)
                x = "OKAY";
        };

        apply_for_each(lref_tuple, lambda);

        assert(std::get<0>(copy_tuple)==11);
        assert(std::get<1>(copy_tuple)==12);
        assert(std::get<2>(copy_tuple)==13);
        assert(std::get<3>(copy_tuple)=="OKAY");

        std::cout << counter++ << ".Pass\n" << std::endl;
    }
    {
        auto&& rref_tuple = std::make_tuple(1,2,3,std::string("okay"));

        auto lambda = []<typename T>(T&& x)
        {
            static_assert(std::is_rvalue_reference_v<T&&>);
            if constexpr(std::same_as<int, std::remove_cvref_t<T>>)
                x+=10;
            else if constexpr(std::same_as<std::string, std::remove_cvref_t<T>>)
                auto temp = std::move(x);
        };

        using T_type = decltype(rref_tuple);

        apply_for_each(std::forward<T_type>(rref_tuple), lambda);

        assert(std::get<0>(rref_tuple)==11);
        assert(std::get<1>(rref_tuple)==12);
        assert(std::get<2>(rref_tuple)==13);
        assert(std::get<3>(rref_tuple).size()==0);

        std::cout << counter++ << ".Pass\n" << std::endl;
    }
    {
        auto empty_tuple = std::make_tuple();

        auto lambda = []<typename T>(T&& x)
        {
            static_assert(true, "In lambda");
        };

        apply_for_each(empty_tuple, lambda);

        std::cout << counter++ << ".Pass\n" << std::endl;
    }
    {
        int x = 5;
        std::string str = "okay";

        auto tuple_tuple = std::make_tuple(std::tie(x, str));

        auto lambda = []<typename T>(T&& x)
        {
            if constexpr(std::same_as<int, std::remove_cvref_t<T>>)
                x+=10;
            else if constexpr(std::same_as<std::string, std::remove_cvref_t<T>>)
                x = "OKAY";
        };

        auto decorated_lambda = [&lambda]<typename T>(T&& x)
        {
            if constexpr(is_tuple<T>)
                apply_for_each(std::forward<T>(x), lambda);
            else
                lambda(std::forward<T>(x));
        };

        apply_for_each(tuple_tuple, decorated_lambda);

        assert(x==15);
        assert(str=="OKAY");

        std::cout << counter++ << ".Pass\n" << std::endl;
    }
}
