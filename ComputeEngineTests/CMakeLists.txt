file(GLOB TEST_SOURCES "*.cpp" "**/*.cpp")
add_executable(ComputeEngineTests ${TEST_SOURCES})
target_link_libraries(ComputeEngineTests PRIVATE project_options project_warnings)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(ComputeEngineTests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME ComputeEngineTests COMMAND ComputeEngineTests)
