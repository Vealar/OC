cmake_minimum_required(VERSION 3.26)
project(ihwShop)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        ihwShop main.cpp
        Customer.h
        Customer.cpp
        Departments.h
        Departments.cpp
)
