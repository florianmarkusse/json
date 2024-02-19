project(json-example LANGUAGES C)

list(APPEND SOURCE_FILES
    "main.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE util-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE parse-framework-${CMAKE_BUILD_TYPE})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE json-${CMAKE_BUILD_TYPE})

