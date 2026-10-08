#pragma once
#include<stdexcept>
#include"DynamicArray.hpp"

template <typename T>
class Stack {
private:
    DynamicArray<T> array_;
public:
    Stack() = default;

    void push(const T& item) {
        int sz = array_.get_size();
        array_.resize(sz+1);
        array_[sz] = item;
    }

    bool empty() const {
        return array_.empty();
    }

    void pop() {
        if (empty()) {
            std::out_of_range("Stack is empty");
        }
        array_.remove_at(array_.get_size() - 1);
    }

    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }

        return array_[array_.get_size() - 1];
    }

    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return array_[array_.get_size() - 1];
    }

    int get_size() const {
        return array_.get_size();
    }
};