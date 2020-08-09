file(GLOB SOURCES "*.cpp" "**/*.cpp")
add_executable(ComputeEngine ${SOURCES})
target_link_libraries(ComputeEngine PRIVATE project_options project_warnings)

if (OpenMP_CXX_FOUND)
  target_link_libraries(ComputeEngine PRIVATE OpenMP::OpenMP_CXX)
elseif (OpenMP_CXX_WORKAROUND_FOUND)
  target_link_libraries(ComputeEngine PRIVATE ${OpenMP_LIBRARY})

  # OpenMP .dll
  set(OpenMP_DllPath "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/x64/bin/libomp.dll")
  add_custom_target(OpenMP_DLL_COPY
      COMMAND ${CMAKE_COMMAND} -E copy ${OpenMP_DllPath} ${CMAKE_BINARY_DIR}/ComputeEngine/Debug/libomp.dll
      COMMAND ${CMAKE_COMMAND} -E copy ${OpenMP_DllPath} ${CMAKE_BINARY_DIR}/ComputeEngine/RelWithDebInfo/libomp.dll
      )

  add_dependencies(ComputeEngine OpenMP_DLL_COPY)
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
