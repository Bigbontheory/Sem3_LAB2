#include "StudentDatabase.hpp"
#include <stdexcept>

void StudentDatabase::append_id(DynamicArray<int>& arr, int id) {
    int current_size = arr.get_size();
    arr.resize(current_size + 1);
    arr[current_size] = id;
}

void StudentDatabase::append_student(DynamicArray<Student>& arr, const Student& student) {
    int current_size = arr.get_size();
    arr.resize(current_size + 1);
    arr[current_size] = student;
}

bool StudentDatabase::remove_id(DynamicArray<int>& arr, int id) {
    for (int i = 0; i < arr.get_size(); ++i) {
        if (arr[i] == id) {
            arr.remove_at(i);
            return true;
        }
    }
    return false;
}

bool StudentDatabase::add(const Student& student) {
    int id = student.get_id();

    if (primary_index_.contains(id)) {
        return false;
    }

    primary_index_.insert(id, student);

    int year = student.get_admission_year();
    if (year_index_.contains(year)) {
        DynamicArray<int>& ids = year_index_.get(year);
        append_id(ids, id);
    } else {
        DynamicArray<int> ids;
        append_id(ids, id);
        year_index_.insert(year, ids);
    }
    return true;
}

bool StudentDatabase::remove(int id) {
    if (!primary_index_.contains(id)) {
        return false;
    }

    int year = primary_index_.get(id).get_admission_year();

    primary_index_.erase(id);

    if (year_index_.contains(year)) {
        DynamicArray<int>& ids = year_index_.get(year);
        remove_id(ids, id);

        if (ids.empty()) {
            year_index_.erase(year);
        }
    }
    return true;
}

void StudentDatabase::clear() {
    primary_index_ = HashTable<int, Student>();
    year_index_ = AVLTree<int, DynamicArray<int>>();
}

bool StudentDatabase::contains(int id) const {
    return primary_index_.contains(id);
}

const Student& StudentDatabase::get_by_id(int id) const {
    return primary_index_.get(id);
}

DynamicArray<Student> StudentDatabase::find_by_admission_year_range(int min_year, int max_year) const {
    DynamicArray<Student> result;

    if (min_year > max_year) {
        return result;
    }

    auto tree_it = year_index_.get_iterator();

    while (tree_it->has_next()) {
        auto current_pair = tree_it -> current();
        int year = current_pair.first;

        if (year > max_year) {
            break;
        }

        if (year >= min_year) {
            const DynamicArray<int>& ids = current_pair.second;
            for (int i = 0; i < ids.get_size(); ++ i) {
                int id = ids[i];
                if (primary_index_.contains(id)) {
                    append_student(result, primary_index_.get(id));
                }
            }
        }

        tree_it -> next();
    }
    return result;
}

int StudentDatabase::size() const noexcept {
    return primary_index_.size();
}

bool StudentDatabase::empty() const noexcept {
    return primary_index_.size() == 0;
}

