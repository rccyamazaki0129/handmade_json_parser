cmake_minimum_required(VERSION 3.20)

project(HandmadeJsonParser CXX)

add_executable(HandmadeJsonParser src/main.cpp)

target_include_directories(HandmadeJsonParser PRIVATE include)

set_target_properties(
    HandmadeJsonParser PROPERTIES
    XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY $(SRCROOT))