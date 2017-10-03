
cmake_minimum_required(VERSION 3.9)

project(irstore)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-std=c++14")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Weverything")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic -Wno-c++98-compat -Wno-padded")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wcast-qual -Wctor-dtor-privacy")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default -Wundef ")
else ()
    message(FATAL_ERROR "Unsupported compiler = ${CMAKE_CXX_COMPILER_ID}. Only gcc & clang are supported.")
endif()

include_directories("${PROJECT_SOURCE_DIR}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    execute_process(COMMAND g++ -print-file-name=libstdc++.a OUTPUT_VARIABLE CXX_ARCHIVE_STDLIB OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "CUSTOM C++ stlib: ${CXX_ARCHIVE_STDLIB}")
endif()

################################################################
#  Library: irstore
################################################################
add_library(irstore STATIC store.cpp irerror.cpp file.cpp)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(irstore "${CXX_ARCHIVE_STDLIB}")
else()
    target_link_libraries(irstore)
endif()

################################################################
#  Executable: storetest 
################################################################
add_executable(storetest storetest.c)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(storetest irstore "-static")
else()
    target_link_libraries(storetest irstore)
endif()
