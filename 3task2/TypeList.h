#pragma once

#include <cstddef>
#include <type_traits>

namespace tl {

template <typename... Ts>
struct TypeList {
    static constexpr std::size_t size = sizeof...(Ts);
};

// Get type by index
// Main template, recyrecevly take a head
template <std::size_t Index, typename... Ts>
struct GetImpl;

// if index == 0 -> return head
template <typename T, typename... Rest>
struct GetImpl<0, T, Rest...> {
    using type = T;
};

// index > 0 -> skip head
template <std::size_t Index, typename T, typename... Rest>
struct GetImpl<Index, T, Rest...> {
    using type = typename GetImpl<Index - 1, Rest...>::type;
};

template <std::size_t Index, typename TypeList>
struct Get;

template <std::size_t Index, typename... Ts>
struct Get<Index, TypeList<Ts...>> {
    using type = typename GetImpl<Index, Ts...>::type;
};

// check the existence of type
// main template
template <typename T, typename... Ts>
struct Contains : std::false_type {};

// head the same as type we find
template <typename T, typename... Rest>
struct Contains<T, T, Rest...> : std::true_type {};

// if head not the finding type -> take next
template <typename T, typename First, typename... Rest>
struct Contains<T, First, Rest...> : Contains<T, Rest...> {};


template <typename T, typename TypeList>
struct ContainsInList;

template <typename T, typename... Ts>
struct ContainsInList<T, TypeList<Ts...>> : Contains<T, Ts...> {};

template <typename T, typename TypeList>
constexpr bool contains_v = ContainsInList<T, TypeList>::value;


// get type's index
template <typename T, typename... Ts>
struct IndexOfImpl;


template <typename T, typename... Rest>
struct IndexOfImpl<T, T, Rest...> {
    static constexpr std::size_t value = 0;
};


template <typename T, typename First, typename... Rest>
struct IndexOfImpl<T, First, Rest...> {
    static constexpr std::size_t value = 1 + IndexOfImpl<T, Rest...>::value;
};

template <typename T, typename TypeList>
struct IndexOf;

template <typename T, typename... Ts>
struct IndexOf<T, TypeList<Ts...>> {
    static constexpr std::size_t value = IndexOfImpl<T, Ts...>::value;
};

template <typename T, typename TypeList>
constexpr std::size_t index_of_v = IndexOf<T, TypeList>::value;

// add the type at the end of the list
template <typename TypeList, typename T>
struct PushBack;

template <typename... Ts, typename T>
struct PushBack<TypeList<Ts...>, T> {
    using type = TypeList<Ts..., T>;
};


// add the type at the start of the list
template <typename TypeList, typename T>
struct PushFront;

template <typename... Ts, typename T>
struct PushFront<TypeList<Ts...>, T> {
    using type = TypeList<T, Ts...>;
};

}