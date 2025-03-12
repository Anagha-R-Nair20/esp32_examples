cmake_minimum_required(VERSION 3.16)

# Add the manually installed micro-ROS component
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(micro_ros_mpu6050)
