cmake_minimum_required(VERSION 3.7)
project(test_mutex)

#set(CMAKE_CXX_COMPILER /opt/rh/devtoolset-6/root/usr/bin/g++)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES test/main.cpp)
add_executable(test_mutex ${SOURCE_FILES})
target_link_libraries(test_mutex pthread)