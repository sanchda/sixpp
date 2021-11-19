########################## <Adapted from Check docs> ###########################
include(FindPkgConfig)
pkg_search_module(CHECK Check)
set(CHECK_INCLUDE_DIRS "${CHECK_INSTALL_DIR}/include")
find_library(CHECK_LIBRARY NAMES check PATHS "${CHECK_INSTALL_DIR}/lib")
set(CHECK_LIBRARIES "${CHECK_LIBRARY}")
include_directories(${CHECK_INCLUDE_DIRS})

########################## </Adapted from Check docs> ##########################

#################################### Tests #####################################
enable_testing()
include_directories(
  ${ROOT}
  ${ROOT}/include
  ${ROOT}/proto)

# Tree
add_executable(check_tree 
  ${SRC}/tree.c
  ${CMAKE_CURRENT_SOURCE_DIR}/check_tree.c)
target_link_libraries(check_tree ${CHECK_LIBRARIES})
add_test(NAME check_tree COMMAND check_tree)

# Profile
add_executable(check_profile
  ${SRC}/profile.c
  ${PPROF_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/check_profile.c)
target_link_libraries(check_profile ${CHECK_LIBRARIES} /usr/lib/x86_64-linux-gnu/libprotobuf.a /usr/lib/x86_64-linux-gnu/libprotobuf-c.a)
add_test(NAME check_profile COMMAND check_profile)
