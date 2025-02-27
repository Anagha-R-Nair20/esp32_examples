# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/opt/esp/idf/components/bootloader/subproject"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/tmp"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/src/bootloader-stamp"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/src"
  "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/micro_ros_espidf_component/examples/esp2_subscriber/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
