# Integration tests CMakeLists.txt

add_executable(integration_tests
    test_main.cpp
    # Add your test files here
)

target_include_directories(integration_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(integration_tests PRIVATE
    # Add your project libraries here
    juce::juce_core
)

add_test(NAME integration_tests COMMAND integration_tests)
