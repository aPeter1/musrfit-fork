# configure_musrfit_version_file.cmake.in:
set(SRC_DIR "/Users/maria/Applications/musrfit")
set(BIN_DIR "/Users/maria/Applications/musrfit/build/src")

# Set variables
set(CMAKE_INSTALL_PREFIX "/usr/local")
set(MUSRFIT_VERSION "")

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --pretty="%h, %ci"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_CURRENT_SHA1
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
  ${SRC_DIR}/cmake/git-revision.h.in
  ${BIN_DIR}/git-revision.h
  @ONLY
)
# EOF
