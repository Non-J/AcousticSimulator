file(GLOB SOURCES "**/*.h" "**/*.cpp")
add_executable(ComputeEngine main.cpp ${SOURCES})
target_link_libraries(ComputeEngine PRIVATE project_options project_warnings)

find_package(OpenMP REQUIRED)
if (OpenMP_CXX_FOUND)
  target_link_libraries(ComputeEngine PRIVATE OpenMP::OpenMP_CXX)
endif ()

find_package(fmt CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(SFML COMPONENTS system window graphics audio network REQUIRED)
target_link_libraries(ComputeEngine PRIVATE FLAC OpenAL OpenGL Vorbis)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE imgui::imgui)

find_package(ImGui-SFML CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE ImGui-SFML::ImGui-SFML)
