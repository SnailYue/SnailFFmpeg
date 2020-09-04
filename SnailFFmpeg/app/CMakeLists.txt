
cmake_minimum_required(VERSION 3.4.1)


#add_library(live
#        SHARED
#        src/main/cpp/RtmpPusher.cpp
#        src/main/cpp/AudioStream.cpp
#        src/main/cpp/VideoStream.cpp)

find_library(log-lib log)


add_library(transform
        SHARED
        src/main/cpp/TransformMedia.cpp
        src/main/cpp/Mp4ToAVI.h)

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

add_library(avutil-55
        SHARED
        IMPORTED)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libavutil-55.so)

add_library(swresample-2
        SHARED
        IMPORTED)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libswresample-2.so)
add_library(avcodec-57
        SHARED
        IMPORTED)
set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libavcodec-57.so)
add_library(avfilter-6
        SHARED
        IMPORTED)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libavfilter-6.so)
add_library(swscale-4
        SHARED
        IMPORTED)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libswscale-4.so)
add_library(avdevice-57
        SHARED
        IMPORTED)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libavdevice-57.so)
add_library(avformat-57
        SHARED
        IMPORTED)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/armeabi-v7a/libavformat-57.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


include_directories(src/main/cpp/include)


target_link_libraries(
#        live
        transform
        x264
        faac
        rtmp
        avcodec-57
        avdevice-57
        avfilter-6
        avformat-57
        avutil-55
        swresample-2
        swscale-4
        ${log-lib})