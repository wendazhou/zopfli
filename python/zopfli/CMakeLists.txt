cmake_minimum_required(VERSION 3.11)
project(pyzopfli)

set(PYBIND11_CPP_STANDARD /std:c++17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/pybind11 ${CMAKE_CURRENT_BINARY_DIR}/third_party/pybind11)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/csrc/)

pybind11_add_module(_zopfli MODULE src/pyzopfli/module.cpp)
target_link_libraries(_zopfli PRIVATE libzopfli)
