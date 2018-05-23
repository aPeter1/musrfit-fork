#--- musredit_qt5 for Qt > 5.0 ------------------------------------------------

message("debug> will eventually deal with Qt > 5.0 musredit, musrStep, musrWiz, ...")

#--- create musrfit-info.h ----------------------------------------------------
configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/musrfit-info.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/musrfit-info.h
)

add_subdirectory(musredit)
#add_subdirectory(musrStep)
#add_subdirectory(musrWiz)
