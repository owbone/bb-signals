# Add the interface library.
add_library(signals INTERFACE)
target_include_directories(signals INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:bb/signals>)

# Install to <prefix>/include/bb/signals.
install(DIRECTORY .
    DESTINATION include/bb/signals
    FILES_MATCHING PATTERN "*.hpp")

# Fake header target, so the headers are included when generating IDE projects.
FILE(GLOB_RECURSE HEADER_FILES "*.hpp")
add_custom_target(headers SOURCES ${HEADER_FILES})
