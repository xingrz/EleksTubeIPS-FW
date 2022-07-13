cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_SOURCE_DIR}/modules/sm74hc595)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(EleksTubeIPS)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
