cmake_minimum_required(VERSION 3.20)
project(LAB2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(HEADERS
        include/IIterator.hpp
        include/IDictionary.hpp
        include/HashSlot.hpp
        include/DynamicArray.hpp
        include/PolynomialHasher.hpp
        include/HashTable.hpp
        include/Hash.hpp
        include/AVLTree.hpp
        include/Stack.hpp
)


add_executable(LAB2 main.cpp ${HEADERS}
        include/Student.hpp
        include/StudentPrint.hpp
        include/StudentDatabase.hpp
        include/StudentDatabase.cpp)
target_include_directories(LAB2 PRIVATE include)


include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()


add_executable(AllTests
        tests/HashTableTests.cpp
        tests/AVLTreeTests.cpp
        tests/DatabaseTests.cpp
        ${HEADERS}
        include/StudentDatabase.cpp
)


target_include_directories(AllTests PRIVATE include)


target_link_libraries(AllTests PRIVATE GTest::gtest GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(AllTests)