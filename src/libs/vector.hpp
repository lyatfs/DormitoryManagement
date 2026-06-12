#pragma once

#include <concepts>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace base {
template <class Type> class Vector {
  private:
    size_t               _size;
    size_t               _cap;
    Type*                _data;
    std::allocator<Type> _alloc{};

    using iterator       = Type*;
    using const_iterator = const Type*;
    using alloc_traits   = std::allocator_traits<decltype(_alloc)>;

  public:
    // Constructor
    constexpr Vector()
        : _size(0), _cap(8), _data(alloc_traits::allocate(_alloc, _cap)) {}
    constexpr Vector(std::initializer_list<Type> vector) : Vector() {
        size_t minCap  = std::bit_ceil(vector.size());
        size_t initCap = (_cap < minCap) ? minCap : _cap;
        reserve(initCap);
        for (auto& val : vector) {
            push_back(val);
        }
    }
    constexpr Vector(const Vector& other)
        : _size(other.size()), _cap(other.capacity()) {
        _data = alloc_traits::allocate(_alloc, _cap);

        for (size_t i = 0; i < _size; i++) {
            alloc_traits::construct(_alloc, _data + i, other[i]);
        }
    };

    // Destructor
    constexpr ~Vector() {
        destroyRange(_data, _data + _size);
        alloc_traits::deallocate(_alloc, _data, _cap);
    }
    constexpr void destroyRange(Type* begin, Type* end) {
        while (begin != end) {
            alloc_traits::destroy(_alloc, begin);
            begin++;
        }
    }

    // Assignment
    constexpr Vector& operator=(const Vector& other) {
        if (this != &other) {
            destroyRange(_data, _data + _size);
            alloc_traits::deallocate(_alloc, _data, _cap);

            _size = other.size();
            _cap  = other.capacity();

            _data = alloc_traits::allocate(_alloc, _data, _cap);
            for (size_t i = 0; i < _size; i++) {
                alloc_traits::construct(_alloc, _data + i, other[i]);
            }
        }
        return *this;
    };

    // Endpoint iterators
    constexpr iterator       begin() noexcept { return _data; }
    constexpr const_iterator begin() const noexcept { return _data; }

    constexpr iterator       end() noexcept { return _data + _size; }
    constexpr const_iterator end() const noexcept { return _data + _size; }

    constexpr const_iterator cbegin() const noexcept { return _data; }
    constexpr const_iterator cend() const noexcept { return _data + _size; }

    // Element access
    constexpr Type front() const {
        if (_size == 0) {
            throw std::out_of_range("index out of range");
        }
        return _data[0];
    }

    constexpr Type back() const {
        if (_size == 0) {
            throw std::out_of_range("index out of range");
        }
        return _data[_size - 1];
    }

    constexpr Type& operator[](size_t index) {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return _data[index];
    }
    constexpr const Type& operator[](size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return _data[index];
    }

    constexpr Type& at(size_t index) {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return _data[index];
    }
    constexpr const Type& at(size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return _data[index];
    }

    // Reserve Vector
    constexpr void reserve(size_t newCap) {
        if (newCap <= _cap) {
            return;
        }

        Type* newData = alloc_traits::allocate(_alloc, newCap);
        for (size_t i = 0; i < _size; i++) {
            alloc_traits::construct(_alloc, newData + i, std::move(_data[i]));
            alloc_traits::destroy(_alloc, _data + i);
        }
        alloc_traits::deallocate(_alloc, _data, _cap);
        _data = newData;
        _cap  = newCap;
    }

    // Append value
    constexpr void push_back(const Type& value) {
        if (_size == _cap) {
            reserve(2 * _cap);
        }
        alloc_traits::construct(_alloc, _data + _size++, value);
    }
    constexpr void insert(size_t index, const Type& value) {
        if (index > _size) {
            return;
        }
        if (index == _size) {
            push_back(value);
            return;
        }

        if (_size == _cap) {
            reserve(2 * _cap);
        }
        alloc_traits::construct(_alloc, _data + _size,
                                std::move(_data[_size - 1]));
        for (size_t i = _size - 1; i > index; i--) {
            _data[i] = std::move(_data[i - 1]);
        }
        _data[index] = value;
        _size++;
    }

    // Remove element
    constexpr void pop_back() {
        if (_size == 0) {
            return;
        }

        alloc_traits::destroy(_alloc, _data + --_size);
    }
    constexpr void erase_at(size_t index) { erase(begin() + index); }

    constexpr void erase(iterator it) {
        if (_size == 0 || it >= end() || it < begin()) {
            return;
        }

        for (iterator i = it; i < end() - 1; i++) {
            *i = std::move(*(i + 1));
        }

        alloc_traits::destroy(_alloc, _data + --_size);
    }
    constexpr void erase(iterator first, iterator last) {
        if (_size == 0 || first > last || begin() > first || end() <= last) {
            return;
        }

        for (size_t i = 0; i < (size_t)(end() - last); i++) {
            *(first + i) = std::move(*(last + 1 + i));
        }

        size_t dist = last - first + 1;
        _size -= dist;
        for (size_t i = 0; i < dist; i++) {
            alloc_traits::destroy(_alloc, _data + _size + i);
        }
    }

    // find element
    constexpr iterator find(const Type& value) {
        for (iterator it = begin(); it != end(); ++it) {
            if (*it == value)
                return it;
        }
        return end();
    }
    constexpr const_iterator find(const Type& value) const {
        for (iterator it = begin(); it != end(); ++it) {
            if (*it == value)
                return it;
        }
        return end();
    }

    // Features
    constexpr size_t size() const noexcept { return _size; }
    constexpr size_t capacity() const noexcept { return _cap; }
    constexpr bool   empty() const noexcept { return _size == 0; }
};
} // namespace base