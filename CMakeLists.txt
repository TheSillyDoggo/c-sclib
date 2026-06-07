cmake_minimum_required(VERSION 3.21)

project(sclib)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LZMA REQUIRED liblzma)
pkg_check_modules(ZSTD REQUIRED libzstd)

include_directories(
    ${LZMA_INCLUDE_DIRS}
    ${ZSTD_INCLUDE_DIRS}
)

file(GLOB SOURCES
    src/*.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} STATIC
    ${LZMA_LIBRARIES}
    ${ZSTD_LIBRARIES}
)