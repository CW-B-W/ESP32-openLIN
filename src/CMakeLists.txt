# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
FILE(GLOB open_lin_c_sources ${CMAKE_SOURCE_DIR}/src/open-LIN-c/*.*)
FILE(GLOB espsoftwareserial_sources ${CMAKE_SOURCE_DIR}/src/ESP32-SoftwareLIN/src/espsoftwareserial/src/*.*)
FILE(GLOB esp32_softwarelin_sources ${CMAKE_SOURCE_DIR}/src/ESP32-SoftwareLIN/src/*.*)

idf_component_register(
        SRCS 
            ${app_sources}
            ${open_lin_c_sources}
            ${esp32_softwarelin_sources}
            ${espsoftwareserial_sources}
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/open-LIN-c
            ${CMAKE_SOURCE_DIR}/src/ESP32-SoftwareLIN/src
            ${CMAKE_SOURCE_DIR}/src/ESP32-SoftwareLIN/src
)
