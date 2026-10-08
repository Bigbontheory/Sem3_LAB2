#pragma once

#include "Student.hpp"
#include "HashTable.hpp"
#include "AVLTree.hpp"
#include "DynamicArray.hpp"

class StudentDatabase {
private:
    HashTable<int, Student> primary_index_;

    AVLTree<int, DynamicArray<int>> year_index_;

    static void append_id(DynamicArray<int>& arr, int id);
    static void append_student(DynamicArray<Student>& arr, const Student& student);
    static bool remove_id(DynamicArray<int>& arr, int id);

public:
    StudentDatabase() = default;
    ~StudentDatabase() = default;

    StudentDatabase(const StudentDatabase& other) = default;
    StudentDatabase& operator=(const StudentDatabase& other) = default;
    StudentDatabase(StudentDatabase&& other) noexcept = default;
    StudentDatabase& operator=(StudentDatabase&& other) noexcept = default;

    bool add(const Student& student);
    bool remove(int id);

    void clear();

    bool contains(int id) const;

    const Student& get_by_id(int id) const;

    DynamicArray<Student> find_by_admission_year_range(int min_year, int max_year) const;

    int size() const noexcept;

    bool empty() const noexcept;
};