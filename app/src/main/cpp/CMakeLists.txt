cmake_minimum_required(VERSION 3.4.1)
project(cuuube)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_library(gljni SHARED
        native-lib.cpp)

# add lib dependencies
target_link_libraries(gljni
        android
        log
        EGL
        GLESv2) #here you can put your opengl linking library.