project(json LANGUAGES C)

set(FLO_SERIALIZER_LIBRARY "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
set(LIBRARY_TYPE "STATIC")

add_library(${FLO_SERIALIZER_LIBRARY} ${LIBRARY_TYPE}
    "src/deserialize.c"
    "src/types/array.c"
    "src/types/bool.c"
    "src/types/common.c"
    "src/types/string.c"
    "src/types/object.c"
    "src/types/uint64.c"
    "src/serialize.c"
)

target_include_directories(${FLO_SERIALIZER_LIBRARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${FLO_SERIALIZER_LIBRARY} PRIVATE util-${CMAKE_BUILD_TYPE})
target_link_libraries(${FLO_SERIALIZER_LIBRARY} PRIVATE parse-framework-${CMAKE_BUILD_TYPE})

if("${BUILD_FUZZ}")
    add_subdirectory(fuzzing)
endif()
if("${BUILD_TESTS}")
    add_subdirectory(tests)
endif()
if("${BUILD_EXAMPLE}")
    add_subdirectory(example)
endif()
