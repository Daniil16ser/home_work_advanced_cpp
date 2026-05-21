#pragma once

#include "TypeList.h"
#include <tuple>
#include <cstddef>
#include <exception>

namespace tl {

template <typename... Ts>
class TypeMap {
    // inner TypeList
    using List = TypeList<Ts...>;

    std::tuple<Ts...> data_;
    // all false initial
    bool present_[sizeof...(Ts)] = {};

public:
    template <typename T>
    void AddValue(const T& value) {
        constexpr std::size_t idx = IndexOf<T, List>::value;
        std::get<idx>(data_) = value;
        present_[idx] = true;
    }

    template <typename T>
    T& GetValue() {
        constexpr std::size_t idx = IndexOf<T, List>::value;
        constexpr std::size_t idx = IndexOf<T, List>::value;
        if (present_[idx] == true){
            return std::get<idx>(data_);
        } else{throw(std::logic_error("Value not present"))}
    }

    template <typename T>
    bool Contains() const {
        constexpr std::size_t idx = IndexOf<T, List>::value;
        return present_[idx];
    }

    template <typename T>
    void RemoveValue() {
        constexpr std::size_t idx = IndexOf<T, List>::value;
        present_[idx] = false;
    }
};

}