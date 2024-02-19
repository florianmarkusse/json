project(util LANGUAGES C)

set(FLO_UTIL_LIBRARY "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")
set(LIBRARY_TYPE "STATIC")

add_library(${FLO_UTIL_LIBRARY} ${LIBRARY_TYPE}
    "src/memory/arena.c"
    "src/file/read.c"
    "src/log.c"
)

target_include_directories(${FLO_UTIL_LIBRARY}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
