cmake_minimum_required(VERSION 2.6)
project(ai_app)

add_definitions(-std=c++11)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

include_directories(
    include
    ${PROJECT_SOURCE_DIR}/3rd/Eigen3/include/eigen3
    ${PROJECT_SOURCE_DIR}/3rd/jsoncpp/include
    ${PROJECT_SOURCE_DIR}/3rd/opencv/include
    ${OpenCV_INCLUDE_DIRS}
    bytetrack/include
    ${PROJECT_SOURCE_DIR}/3rd/onnxruntime/include
    ${PROJECT_SOURCE_DIR}/3rd/onnxruntime/include/onnxruntime
    ${PROJECT_SOURCE_DIR}/3rd/onnxruntime/include/onnxruntime/core/session
)

link_directories(
    ${PROJECT_SOURCE_DIR}/3rd/jsoncpp/lib
    ${PROJECT_SOURCE_DIR}/3rd/opencv/x64/vc16/lib
    ${PROJECT_SOURCE_DIR}/3rd/onnxruntime/lib
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O0 -pthread -w -g")

file(GLOB_RECURSE cpp_srcs ${PROJECT_SOURCE_DIR}/bytetrack/src/*.cpp)
add_library(bytetrack STATIC ${cpp_srcs} "include/onnx_task.hpp" "include/yolov8_onnx.hpp" "include/push_task.h")

add_executable(test_http_client src/test_http_client.cpp)
add_executable(ai_app src/app_task_base.cpp src/det_task.cpp src/http_server.cpp src/app_server.cpp src/rtsp_task.cpp 
                src/track_task.cpp src/output_task.cpp
                src/main.cpp "include/onnx_task.hpp" "include/yolov8_onnx.hpp" "include/push_task.h")
set(OpenCV_LIBS opencv_core490d opencv_highgui490d opencv_imgproc490d opencv_calib3d490d opencv_imgcodecs490d opencv_video490d opencv_videoio490d)
find_package(glog REQUIRED)
target_link_libraries(ai_app ${OpenCV_LIBS} jsoncpp.lib bytetrack onnxruntime glog::glog)
