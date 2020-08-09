if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND WIN32)
  include(cmake/ucm.cmake)
  # Enable exception
  ucm_add_flags(/EHsc)

  # OpenMP compiler flag
  ucm_add_flags(-openmp)

  # OpenMP library
  find_library(OpenMP_LIBRARY
      NAMES libomp libgomp libiomp5
      HINTS "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/x64/lib")

  set(OpenMP_CXX_WORKAROUND_FOUND TRUE)
else ()
  find_package(OpenMP)
endif ()