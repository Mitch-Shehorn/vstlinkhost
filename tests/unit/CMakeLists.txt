# Unit tests CMakeLists.txt

add_executable(unit_tests
    test_main.cpp
    # Add your test files here
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(unit_tests PRIVATE
    # Add your project libraries here
    juce::juce_core
)

add_test(NAME unit_tests COMMAND unit_tests)
