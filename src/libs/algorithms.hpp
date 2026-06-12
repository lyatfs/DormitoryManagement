#pragma once

#include "vector.hpp"
#include <random>
#include <type_traits>

/*
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Default comparation
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*/
template <typename Type> constexpr void swap(Type& a, Type& b) noexcept {
    Type tmp = a;
    a        = b;
    b        = tmp;
}

/*
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Default comparation
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*/
constexpr auto defaultCompare = [](auto& a, auto& b) -> bool { return a < b; };

using LambdaType = decltype(defaultCompare);

/*
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Sorting
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*/
std::mt19937 gen(std::random_device{}());

template <typename Array, typename Compare = LambdaType>
void sort(Array& data, size_t left, size_t right,
               Compare comp = defaultCompare) {
    using Type = std::remove_reference_t<decltype(data[0])>;

    if (left < right) {
        std::uniform_int_distribution<size_t> dist(left, right);

        size_t pivotIndex = dist(gen);
        Type   pivot      = data[pivotIndex];
        swap(data[pivotIndex], data[right]);

        int i = left;
        int j = right - 1;
        while (i <= j) {
            while (i <= j && comp(data[i], pivot))
                i++;
            while (i <= j && comp(pivot, data[j]))
                j--;

            if (i <= j)
                swap(data[i++], data[j--]);
            else
                break;
        }
        swap(data[i], data[right]);

        if (i > (int)left)
            sort(data, left, i - 1, comp);
        if (i < (int)right)
            sort(data, i + 1, right, comp);
    }
}
template <typename Array, typename Compare = LambdaType>
void sort(Array& data, Compare comp = defaultCompare) {
    if (std::size(data) <= 1)
        return;

    sort(data, 0, std::size(data) - 1, comp);
}

/*
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
Binary search
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────
*/
template <typename Container, typename Type, typename Compare = LambdaType>
constexpr auto binarySearch(const Container& data, const Type& value,
                            Compare comp = defaultCompare)
    -> decltype(data.begin()) {
    size_t left  = 0;
    size_t right = std::size(data);

    while (left < right) {
        size_t mid = (right + left) / 2;

        if (comp(data[mid], value))
            left = mid + 1;
        else
            right = mid;
    }

    return data.begin() + left;
}