include(ExternalProject)
ExternalProject_Add(libsixel_extern
  PREFIX vendor
  BUILD_IN_SOURCE 1
  SVN_REPOSITORY "https://github.com/saitoha/libsixel/trunk/"
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND "./configure"
  BUILD_COMMAND "make"
  INSTALL_COMMAND "")

set(LIBSIXEL_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/vendor/src/libsixel_extern/include)
set(LIBSIXEL_LIB ${CMAKE_CURRENT_BINARY_DIR}/vendor/src/libsixel_extern/src/.libs)
add_library(libsixel STATIC IMPORTED)
set_target_properties(libsixel PROPERTIES IMPORTED_LOCATION ${LIBSIXEL_LIB}/libsixel.a)
add_dependencies(libsixel libsixel_extern)
