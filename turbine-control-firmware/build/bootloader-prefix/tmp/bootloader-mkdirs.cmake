# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/nicholas/esp-idf/components/bootloader/subproject"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/tmp"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/src/bootloader-stamp"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/src"
  "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/nicholas/turbine-control-firmware/turbine-control-firmware/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
