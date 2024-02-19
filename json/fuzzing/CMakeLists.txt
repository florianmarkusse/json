project(json-fuzzing LANGUAGES C)

list(APPEND FUZZING_SOURCE_FILES
    "fuzzing.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
add_executable(${EXECUTABLE_NAME} ${FUZZING_SOURCE_FILES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE util-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE parse-framework-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE json-${CMAKE_BUILD_TYPE})
