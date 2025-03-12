# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/esp/micro_ros_mpu6050/components/micro_ros/esp32_toolchain.cmake"
  "/esp/micro_ros_mpu6050/components/micro_ros/include"
  "/esp/micro_ros_mpu6050/components/micro_ros/micro_ros_dev"
  "/esp/micro_ros_mpu6050/components/micro_ros/micro_ros_src"
  )
endif()
