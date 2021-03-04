file(GLOB PLEVEL_SRC *c)

include_directories(${PMDK_INCLUDE_DIR})
link_directories(${PMDK_LIB_DIR})

add_executable(plevel ${PLEVEL_SRC})
target_link_libraries(plevel m pmem pmemobj)