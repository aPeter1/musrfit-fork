#--- musrStep for Qt > 5.0 ----------------------------------------------------

set(qt_libs Qt5::Core Qt5::Widgets Qt5::Svg)

set(musrStep_src
  PMusrStep.cpp
  musrStep.cpp
)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

#[==[ 
# as35 currently CMAKE_AUTOUIC -> ON doesn't work since it requires the ui-files
#      in the same directory as the cpp-files.
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#]==]
set(CMAKE_AUTOUIC OFF)

# add qt/rcc
qt5_add_resources(musrStep_rcc musrStep.qrc)

add_executable(musrStep 
  ${musrStep_src}
  ${musrStep_rcc}
)

target_include_directories(musrStep
  BEFORE PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)

target_link_libraries(musrStep ${qt_libs})

#--- installation info --------------------------------------------------------
install(
  TARGETS
    musrStep
  RUNTIME DESTINATION
    bin
)

