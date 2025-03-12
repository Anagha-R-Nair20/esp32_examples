# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/opt/esp/idf/components/bootloader/subproject"
  "/esp/micro_ros_mpu6050/build/bootloader"
  "/esp/micro_ros_mpu6050/build/bootloader-prefix"
  "/esp/micro_ros_mpu6050/build/bootloader-prefix/tmp"
  "/esp/micro_ros_mpu6050/build/bootloader-prefix/src/bootloader-stamp"
  "/esp/micro_ros_mpu6050/build/bootloader-prefix/src"
  "/esp/micro_ros_mpu6050/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/esp/micro_ros_mpu6050/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/esp/micro_ros_mpu6050/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
