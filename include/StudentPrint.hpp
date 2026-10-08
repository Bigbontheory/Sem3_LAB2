#pragma once

#include <iostream>
#include "Student.hpp"

inline std::ostream& operator<<(std::ostream& os, const Student& student) {
    os << "Student(id=" << student.get_id()
       << ", name=\"" << student.get_full_name() << "\""
       << ", group=\"" << student.get_group() << "\""
       << ", admission_year=" << student.get_admission_year()
       << ", gpa=" << student.get_gpa() << ")";
    return os;
}

