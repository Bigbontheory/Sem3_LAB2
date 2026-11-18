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
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.9b)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw imgui)
find_package(OpenGL REQUIRED)
add_library(imgui_backend STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_backend PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui_backend PUBLIC glfw OpenGL::GL)
target_link_libraries(LAB2 PRIVATE imgui_backend)


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
