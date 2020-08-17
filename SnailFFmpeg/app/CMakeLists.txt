cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

include_directories(${CMAKE_SOURCE_DIR}/libs/armeabi-v7a)

set(DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libavcodec.so)

add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libavdevice.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libswscale.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/armeabi/libavfilter.so)

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
        ${CMAKE_SOURCE_DIR}/src/main/cpp/VideoStream.cpp
        ${CMAKE_SOURCE_DIR}/src/main/cpp/SafeQueue.cpp
        )

find_library(log-lib log)

target_link_libraries(live x264 faac rtmp
        avcodec avdevice avformat avutil swresample swscale
        ${log-lib})