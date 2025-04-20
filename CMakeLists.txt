cmake_minimum_required(VERSION 3.10)
project(ATLASCollisionDataAnalysis)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add PDCurses include directory
include_directories(${CMAKE_SOURCE_DIR}/include/pdcurses)

# Source files
add_executable(analysis
        src/main.cpp
        src/DataLoader.cpp
        src/KDTree.cpp
        src/GridBucketing.cpp
)

# Link PDCurses library
target_link_libraries(analysis PRIVATE ${CMAKE_SOURCE_DIR}/lib/pdcurses.a)