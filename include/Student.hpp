#pragma once
#include <string>

class Student {
private:
    int id_;
    std::string first_name_;
    std::string last_name_;
    std::string group_;
    int admission_year_;
    double gpa_;

public:
    Student() = default;
    Student(int id,
            std::string first_name,
            std::string last_name,
            std::string group,
            int admission_year,
            double gpa)
    : id_(id),
      first_name_(std::move(first_name)),
      last_name_(std::move(last_name)),
      group_(std::move(group)),
      admission_year_(admission_year),
      gpa_(gpa) {}

    int get_id() const {
        return id_;
    }
    const std::string& get_first_name() const {
        return first_name_;
    }
    const std::string& get_last_name() const {
        return last_name_;
    }
    std::string get_full_name() const {
        return first_name_ + " " + last_name_;
    }
    const std::string& get_group() const {
        return group_;
    }
    int get_admission_year() const {
        return admission_year_;
    }
    double get_gpa() const {
        return gpa_;
    }

    bool operator==(const Student& other) const {
        return (id_ == other.id_ &&
                first_name_ == other.first_name_ &&
                last_name_ == other.last_name_ &&
                group_ == other.group_ &&
                admission_year_ == other.admission_year_ &&
                gpa_ == other.gpa_);
    }
    bool operator!=(const Student& other) const {
        return !(*this == other);
    }

};