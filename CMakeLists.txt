cmake_minimum_required(VERSION 3.17)
project(vk_mandelbrot)

#App info
set(APP_BINARY_NAME mandelbrot)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 1)
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
find_package(Vulkan REQUIRED)
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
-mavx2 -mbmi -mbmi2 -mf16c -mfma -mfsgsbase \
-mlzcnt -mmovbe -mpclmul -mpopcnt -mrdrnd \
-mxsave -mxsaveopt -pipe")
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fstack-protector-strong -fstack-clash-protection \
        -fcf-protection=full -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -fstack-protector-strong -fstack-clash-protection \
        -fcf-protection=full -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-pie -Wl,-z,relro -Wl,-z,now -Wl,-z,defs \
        -Wl,-z,noexecstack")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fasynchronous-unwind-tables \
        -fno-omit-frame-pointer" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fasynchronous-unwind-tables \
        -fno-omit-frame-pointer" CACHE STRING "" FORCE)
endif ()

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

#Setup imgui
set(IMGUI_FOLDER ${CMAKE_SOURCE_DIR}/external/imgui)
add_library(imgui_glfw_vulkan STATIC
        ${IMGUI_FOLDER}/backends/imgui_impl_glfw.cpp
        ${IMGUI_FOLDER}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_FOLDER}/imgui.cpp
        ${IMGUI_FOLDER}/imgui_draw.cpp
        ${IMGUI_FOLDER}/imgui_tables.cpp
        ${IMGUI_FOLDER}/imgui_widgets.cpp)
target_include_directories(imgui_glfw_vulkan
        PUBLIC
        ${IMGUI_FOLDER}
        ${IMGUI_FOLDER}/backend)
add_dependencies(imgui_glfw_vulkan
        glfw)
target_link_libraries(imgui_glfw_vulkan
        glfw)

#Project libs
add_subdirectory(libraries/io_manager)
add_subdirectory(libraries/engine)
add_subdirectory(libraries/vulkan_tools)
add_subdirectory(libraries/vulkan_instance)
add_subdirectory(libraries/vulkan_assets)
add_subdirectory(libraries/vulkan_renderer)
add_subdirectory(libraries/app_info)
add_subdirectory(libraries/ui)
add_subdirectory(shaders/toScreen)
add_subdirectory(shaders/mandelbrot)

#Main binary
add_executable(${APP_BINARY_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private/mandelbrot.cpp)
target_include_directories(${APP_BINARY_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/binary/private)
target_link_libraries(${APP_BINARY_NAME} fmt iomanager engine vulkan imgui_glfw_vulkan ui)
add_dependencies(${APP_BINARY_NAME} fmt iomanager engine mandelbrot_shader toScreen_shader imgui_glfw_vulkan ui)
set_target_properties(${APP_BINARY_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)
target_compile_options(${APP_BINARY_NAME} PRIVATE -Wall -Wextra -Werror)
