cmake_minimum_required (VERSION 2.8.12)
project (RiDyL)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Library
add_library(RiDyL SHARED src/import.cpp src/RiDyL.cpp src/util.hpp include/RiDyL.hpp include/RiDyL/_config.hpp include/RiDyL/import.hpp)
target_include_directories(RiDyL PUBLIC include)

# Link Libraries
target_link_libraries(RiDyL log dl)
