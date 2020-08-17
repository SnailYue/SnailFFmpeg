cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

include_directories(${CMAKE_SOURCE_DIR}/libs/armeabi-v7a)

add_library(faac
        STATIC
        IMPORTED)
set_target_properties(faac
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libfaac.a)

add_library(rtmp
        STATIC
        IMPORTED)
set_target_properties(rtmp
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/librtmp.a)

add_library(x264
        STATIC
        IMPORTED)
set_target_properties(x264
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libx264.a)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library(live
        SHARED
        ${CMAKE_SOURCE_DIR}/src/main/cpp/RtmpPusher.cpp
        ${CMAKE_SOURCE_DIR}/src/main/cpp/AudioStream.cpp
        ${CMAKE_SOURCE_DIR}/src/main/cpp/VideoStream.cpp)

find_library(log-lib log)

target_link_libraries(live x264 faac rtmp ${log-lib})