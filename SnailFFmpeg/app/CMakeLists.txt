cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

set(DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libavcodec.so)

add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libavdevice.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libswscale.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${DIR}/armeabi/libavfilter.so)


add_library(native-lib
        SHARED
        ${CMAKE_SOURCE_DIR}/src/main/cpp/include/native_lib.cpp)
add_library(init-lib
        SHARED
        ${CMAKE_SOURCE_DIR}/src/main/cpp/include/init_lib.cpp)

find_library(log-lib log)

target_link_libraries(native-lib init-lib
        avcodec avdevice avformat avutil swresample swscale
        ${log-lib})