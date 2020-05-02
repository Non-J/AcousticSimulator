add_executable(ComputeEngine
        main.cpp DataStore.h DataStore.cpp
        imgui_stdlib/imgui_stdlib.cpp
        UserInterface/UserInterface.h UserInterface/MenuBar.cpp UserInterface/SetupStyle.cpp
        UserInterface/TransducerConfigurationWidget.cpp UserInterface/SimulationControlWidget.cpp
        UserInterface/BenchmarkingWidget.cpp UserInterface/RunSimulationWidget.cpp UserInterface/Colors.h
        Computation/Vec3.h Computation/Vec3.cpp Computation/BlockStorage.h Computation/Simulator.h Computation/Simulator.cpp)

target_link_libraries(ComputeEngine PRIVATE project_options project_warnings)

find_package(OpenMP REQUIRED)
target_link_libraries(ComputeEngine PRIVATE OpenMP::OpenMP_CXX)

find_package(docopt CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE docopt_s)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE fmt::fmt fmt::fmt-header-only)

find_package(GSL REQUIRED)
target_link_libraries(ComputeEngine PRIVATE GSL::gsl GSL::gslcblas)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

find_package(SFML COMPONENTS system window graphics audio network REQUIRED)
target_link_libraries(ComputeEngine PRIVATE FLAC OpenAL OpenGL Vorbis)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE imgui::imgui)

find_package(ImGui-SFML CONFIG REQUIRED)
target_link_libraries(ComputeEngine PRIVATE ImGui-SFML::ImGui-SFML)
