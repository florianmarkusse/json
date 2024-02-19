project(json-tests LANGUAGES C)

list(APPEND TEST_SOURCE_FILES
    "src/main.c"
    "src/test/unit.c"
    "src/test/string-deserialization.c"
    "src/test/uint64-deserialization.c"
    "src/test/struct-deserialization.c"
    "src/test/inside-struct-deserialization.c"
    "src/test/integration-deserialization.c"
    "src/comparison-test.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

add_executable(${EXECUTABLE_NAME} ${TEST_SOURCE_FILES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE util-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE parse-framework-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE json-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE test-framework-${CMAKE_BUILD_TYPE})
