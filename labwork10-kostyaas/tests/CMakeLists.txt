include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  itmoscript_tests
  function_test.cpp
  types_test.cpp
  loop_and_branch_test.cpp
    lexical_tests.cpp
    syntactic_tests.cpp
    semantic_tests.cpp
    integration_tests.cpp
    performance_tests.cpp
)

target_link_libraries(
    itmoscript_tests
    interpreter
        lexical_analyser
        semantic_analyser
        syntactic_analyser
  GTest::gtest_main
)

target_include_directories(itmoscript_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(itmoscript_tests)