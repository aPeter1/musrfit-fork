# configure_mupp_version_file.cmake.in:
set(SRC_DIR "/Users/maria/Applications/musrfit/src/musredit_qt6/mupp")
set(BIN_DIR "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp")

# Set MUPP variables
set(CMAKE_INSTALL_PREFIX "/usr/local")
set(MUPP_VERSION "1.0.0")

configure_file(
  ${SRC_DIR}/cmake/mupp_version.h.in
  ${BIN_DIR}/mupp_version.h 
  @ONLY
)
# EOF
