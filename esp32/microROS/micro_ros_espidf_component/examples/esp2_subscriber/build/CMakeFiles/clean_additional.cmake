# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/micro_ros_espidf_component/esp32_toolchain.cmake"
  "/micro_ros_espidf_component/include"
  "/micro_ros_espidf_component/micro_ros_dev"
  "/micro_ros_espidf_component/micro_ros_src"
  )
endif()
