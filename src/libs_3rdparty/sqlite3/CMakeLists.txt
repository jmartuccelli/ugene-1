cmake_minimum_required(VERSION 3.4)
project(ugenedb)

include_directories(src)

file(GLOB_RECURSE SRCS src/*.c)
file(GLOB_RECURSE HDRS src/*.h)

add_definitions(
        -DSQLITE_ENABLE_COLUMN_METADATA
        -DSQLITE_ENABLE_RTREE
        -DSQLITE_OMIT_LOAD_EXTENSION
        -DTHREADSAFE
)

add_library(ugenedb SHARED ${HDRS} ${SRCS})

target_link_libraries(ugenedb)
