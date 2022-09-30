cmake_minimum_required(VERSION 3.17)
project(vk_mandelbrot)

#App info
set(APP_BINARY_NAME mandelbrot)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)

#Detecting + setting global LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_output)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND lto_supported)
    message("Compiling with LTO enabled")
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message("LTO not supported")
endif ()

#Runtime resources
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#Checking Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
find_program(GLSLC_PROGRAM glslc REQUIRED FATAL_ERROR)

#Setting global C/CXX compile options
set(TARGET_ARCH_OPTIMIZATION "native" CACHE STRING "Can be either native,x86-64,x86-64-v2,x86-64-v3,x86-64-v4")

#Compiler support for x86_64 feature level
set(USE_X86_64_PROFILE TRUE)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.1.0")
        set(USE_X86_64_PROFILE FALSE)
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.0.0")
        set(USE_X86_64_PROFILE FALSE)
    endif ()
else ()
    set(USE_X86_64_PROFILE FALSE)
endif ()

#Setting C/CXX optimization flags
if (${TARGET_ARCH_OPTIMIZATION} MATCHES "native")
    message("Build binary for current computer arch")
    set(CMAKE_CXX_FLAGS "-march=native -pipe")
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
elseif (${USE_X86_64_PROFILE} AND ${TARGET_ARCH_OPTIMIZATION} MATCHES "x86-64|x86-64-v2|x86-64-v3|x86-64-v4")
    message("Build binary for profile: ${TARGET_ARCH_OPTIMIZATION}")
    set(CMAKE_CXX_FLAGS "-march=${TARGET_ARCH_OPTIMIZATION} -pipe")
    if (${TARGET_ARCH_OPTIMIZATION} MATCHES "x86-64-v3|x86-64-v4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpclmul -mfsgsbase -mrdrnd")
    endif ()
    set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
else ()
    if (${TARGET_ARCH_OPTIMIZATION} MATCHES "x86-64-v3|x86-64-v4")
        message("Build binary with AVX2 profile")
        set(CMAKE_CXX_FLAGS "-march=core2 -mtune=haswell \
-mavx -mavx2 -mbmi -mbmi2 -mf16c -mfma -mfsgsbase \
-mlzcnt -mmovbe -mpclmul -mpopcnt -mrdrnd \
-msse4 -msse4.1 -msse4.2 -mxsave -mxsaveopt \
-pipe")
        set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
    elseif (${TARGET_ARCH_OPTIMIZATION} MATCHES "x86-64-v2")
        message("Build binary with SSE4 profile")
        set(CMAKE_CXX_FLAGS "-march=core2 -mtune=nehalem \
-msse4 -msse4.1 -msse4.2 -mpopcnt -pipe")
        set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
    else ()
        message("No arch optimization selected")
        set(CMAKE_CXX_FLAGS "-pipe")
        set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
    endif ()
endif ()

#Security settings
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fstack-protector-strong -D_FORTIFY_SOURCE=2" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fstack-protector-strong -D_FORTIFY_SOURCE=2" CACHE STRING "" FORCE)
endif ()

message(${CMAKE_CXX_FLAGS})

#Setup glm
add_library(glm INTERFACE IMPORTED)
set_target_properties(glm PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)

#Setup GLFW
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_INSTALL "" OFF)
option(BUILD_SHARED_LIBS "" OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_BINARY_DIR}/external/glfw/build)

#Setup stb
add_library(stb INTERFACE IMPORTED)
set_target_properties(stb PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)

#Setup fmt
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/fmt ${CMAKE_BINARY_DIR}/external/fmt/build)

#Project libs
add_subdirectory(libraries/io_manager)
add_subdirectory(libraries/engine)
add_subdirectory(libraries/vulkan_renderer)
add_subdirectory(libraries/app_info)
add_subdirectory(shaders/toScreen)
add_subdirectory(shaders/mandelbrot)

#Main binary
add_executable(${APP_BINARY_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private/mandelbrot.cpp)
target_include_directories(${APP_BINARY_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private)
target_link_libraries(${APP_BINARY_NAME} fmt iomanager engine vulkan)
add_dependencies(${APP_BINARY_NAME} fmt iomanager engine mandelbrot_shader toScreen_shader)
set_target_properties(${APP_BINARY_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
target_compile_options(${APP_BINARY_NAME} PRIVATE -Wall -Wextra -Werror)
