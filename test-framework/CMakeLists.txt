project(test-framework LANGUAGES C)

set(FLO_TEST_LIBRARY "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
set(LIBRARY_TYPE "STATIC")

add_library(${FLO_TEST_LIBRARY} ${LIBRARY_TYPE}
    "src/test.c"
    "src/expectations.c"
)

target_include_directories(${FLO_TEST_LIBRARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${FLO_TEST_LIBRARY} PRIVATE util-${CMAKE_BUILD_TYPE})

if("${BUILD_TESTS}")
    add_subdirectory(tests)
endif()

