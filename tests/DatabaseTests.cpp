#include <gtest/gtest.h>
#include "StudentDatabase.hpp"


static Student make_student(int id, int year, const std::string& name = "Test") {

    return Student(id, name, "Surname", "Group-1", year, 4.5);
}

TEST(StudentDatabaseTest, InitialStateIsEmpty) {
    StudentDatabase db;

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_FALSE(db.contains(1));
    EXPECT_THROW(db.get_by_id(1), std::out_of_range);
}

TEST(StudentDatabaseTest, AddAndGetById) {
    StudentDatabase db;

    Student s1 = make_student(101, 2021);
    Student s2 = make_student(102, 2022);

    EXPECT_TRUE(db.add(s1));
    EXPECT_TRUE(db.add(s2));

    EXPECT_EQ(db.size(), 2);
    EXPECT_FALSE(db.empty());

    EXPECT_TRUE(db.contains(101));
    EXPECT_TRUE(db.contains(102));
    EXPECT_FALSE(db.contains(999));

    EXPECT_EQ(db.get_by_id(101).get_id(), 101);
    EXPECT_EQ(db.get_by_id(102).get_admission_year(), 2022);
}

TEST(StudentDatabaseTest, AddDuplicateIdFails) {
    StudentDatabase db;

    Student s1 = make_student(101, 2020);
    Student s1_duplicate = make_student(101, 2024);

    EXPECT_TRUE(db.add(s1));
    EXPECT_FALSE(db.add(s1_duplicate));

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.get_by_id(101).get_admission_year(), 2020);
}

TEST(StudentDatabaseTest, RemoveStudentAndClearYearIndex) {
    StudentDatabase db;

    Student s1 = make_student(101, 2021);
    Student s2 = make_student(102, 2021);

    db.add(s1);
    db.add(s2);

    EXPECT_FALSE(db.remove(999));
    EXPECT_EQ(db.size(), 2);

    // Удаление первого студента года 2021
    EXPECT_TRUE(db.remove(101));
    EXPECT_EQ(db.size(), 1);
    EXPECT_FALSE(db.contains(101));
    EXPECT_TRUE(db.contains(102));
    EXPECT_THROW(db.get_by_id(101), std::out_of_range);

    EXPECT_TRUE(db.remove(102));
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);

    auto result = db.find_by_admission_year_range(2021, 2021);
    EXPECT_EQ(result.get_size(), 0);
}

TEST(StudentDatabaseTest, FindByAdmissionYearRange) {
    StudentDatabase db;

    db.add(make_student(1, 2018));
    db.add(make_student(2, 2020));
    db.add(make_student(3, 2020));
    db.add(make_student(4, 2022));
    db.add(make_student(5, 2025));

    auto res1 = db.find_by_admission_year_range(2019, 2022);
    EXPECT_EQ(res1.get_size(), 3);

    auto res2 = db.find_by_admission_year_range(2020, 2020);
    EXPECT_EQ(res2.get_size(), 2);

    auto res3 = db.find_by_admission_year_range(2030, 2040);
    EXPECT_EQ(res3.get_size(), 0);
}

TEST(StudentDatabaseTest, FindByAdmissionYearRangeEdgeCases) {
    StudentDatabase db;

    db.add(make_student(1, 2020));
    db.add(make_student(2, 2021));

    auto res_invalid = db.find_by_admission_year_range(2025, 2010);
    EXPECT_EQ(res_invalid.get_size(), 0);

    StudentDatabase empty_db;
    auto res_empty = empty_db.find_by_admission_year_range(1900, 2100);
    EXPECT_EQ(res_empty.get_size(), 0);
}

TEST(StudentDatabaseTest, ClearDatabase) {
    StudentDatabase db;

    db.add(make_student(1, 2020));
    db.add(make_student(2, 2021));
    db.add(make_student(3, 2022));

    EXPECT_EQ(db.size(), 3);

    db.clear();

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_FALSE(db.contains(1));
    EXPECT_EQ(db.find_by_admission_year_range(2000, 2030).get_size(), 0);

    EXPECT_TRUE(db.add(make_student(10, 2024)));
    EXPECT_EQ(db.size(), 1);
    EXPECT_TRUE(db.contains(10));
}