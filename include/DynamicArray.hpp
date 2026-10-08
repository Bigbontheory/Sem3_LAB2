#pragma once

template<typename T>
class DynamicArray {
public:
    DynamicArray();
    explicit DynamicArray(int size);
    DynamicArray(const T* items, int count);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray(DynamicArray<T>&& other) noexcept;
    ~DynamicArray();

    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;

    T& get(int index);
    const T& get(int index) const;
    int get_size() const;

    //operations
    void set (int index, const T& value);
    void resize(int new_size);
    void remove_at(int index);

    T& operator[](int index);
    const T& operator[](int index) const;

    bool operator==(const DynamicArray<T>& other) const;
    bool operator!=(const DynamicArray<T>& other) const;

    bool empty() const {
        return size == 0;
    }

private:
    T* data;
    int size;
    int capacity;
};

#include "DynamicArray.tpp"