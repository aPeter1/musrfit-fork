#--- musredit_qt6 for Qt > 6.0 ------------------------------------------------

#--- create musrfit-info.h ----------------------------------------------------
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/musrfit-info.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/musrfit-info.h
)

add_subdirectory(musredit)
add_subdirectory(musrStep)
add_subdirectory(musrWiz)
add_subdirectory(mupp)
