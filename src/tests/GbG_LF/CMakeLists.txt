cmake_minimum_required(VERSION 2.8)

project(GbG_LF)

set(DependOnLibs gsl)
set(DependOnLibs gslcblas)

add_executable(GbG_LF GbG_LF.cpp)

target_link_libraries(GbG_LF gsl gslcblas)

