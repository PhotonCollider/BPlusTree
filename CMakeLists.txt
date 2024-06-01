# specify the minimum version of CMake that is supported
cmake_minimum_required(VERSION 3.10)

# include a project name
project(BPlusTree)

# set C++ Version & executable output path
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# make your compiler aware of header directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# create a variable which includes needed source files
set(MY_SOURCES
  # ${PROJECT_SOURCE_DIR}/src/test.cpp
  ${PROJECT_SOURCE_DIR}/src/code.cpp
)

# specify an executable, 
# build from the specified source files
add_executable(code ${MY_SOURCES})