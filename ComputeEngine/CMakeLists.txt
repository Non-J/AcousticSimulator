add_executable(ComputeEngine main.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE fmt::fmt fmt::fmt-header-only)

find_package(GSL REQUIRED)
target_link_libraries(ComputeEngine PRIVATE GSL::gsl GSL::gslcblas)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE imgui::imgui)

