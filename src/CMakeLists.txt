file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_include_directories(ONYX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(ONYX PRIVATE ${SRC})
