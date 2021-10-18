cmake_minimum_required(VERSION 3.9.2)
project(cuasMQLib VERSION 1.0.1 DESCRIPTION "TODO: cuasMQLib description")


add_library(cuasMQLib INTERFACE)
add_subdirectory(include/cppkafka)

target_include_directories(cuasMQLib INTERFACE include/)
target_include_directories(cuasMQLib INTERFACE src/)
