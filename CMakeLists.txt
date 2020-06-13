cmake_minimum_required(VERSION 3.13)

# Project
project(AcousticSimulator)
include(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" FALSE)
  if (ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(project_options INTERFACE -ftime-trace)
  endif ()
endif ()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# enable cache system
include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# PCH
option(ENABLE_PCH "Enable Precompiled Headers" FALSE)
if (ENABLE_PCH)
  target_precompile_headers(project_options INTERFACE <vector> <string> <map> <utility>)
endif ()

# packages
set(PACKAGES "")
list(APPEND PACKAGES "fmt")
list(APPEND PACKAGES "imgui")
list(APPEND PACKAGES "ImGui-SFML")
list(APPEND PACKAGES "nlohmann-json")
list(APPEND PACKAGES "SFML")

# static linking
if (ENABLE_STATIC_LINKING)
  include(cmake/ucm.cmake)
  ucm_set_runtime(STATIC)
  set(VCPKG_TARGET_TRIPLET "x64-windows-static")
endif ()

# package manager
include(cmake/pmm.cmake)
pmm(VCPKG REVISION "2020.04" TRIPLET "${VCPKG_TARGET_TRIPLET}" REQUIRES "${PACKAGES}")

# Sources
add_subdirectory(ComputeEngine)