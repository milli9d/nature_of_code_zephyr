zephyr_library_named(random_walker)

# add library sources
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_LIST_DIR}/src/*.c*)
zephyr_library_sources(${srcs})
zephyr_library_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# share headers with the application
zephyr_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)