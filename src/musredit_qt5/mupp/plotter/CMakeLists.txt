#--- mupp_plotter -------------------------------------------------------------
message("-- Handle the plotter part of mupp")

#--- instruct cmake NOT to run moc automatically when needed anymore ----------
set(CMAKE_AUTOMOC OFF)

#--- headers ------------------------------------------------------------------
set(HEADER
  mupp_plot.h
  PMuppCanvas.h
  PMuppCanvasLinkDef.h
  PMuppStartupHandler.h
  PMuppStartupHandlerLinkDef.h
)

#--- sources ------------------------------------------------------------------
set(SOURCE
  mupp_plot.cpp
  PMuppCanvas.cpp 
  PMuppStartupHandler.cpp 
)

#--- dictionary generated sources ---------------------------------------------
set ( PLOT_DIR
  ${CMAKE_CURRENT_BINARY_DIR}
)

#--- generate necessary dictionaries ------------------------------------------
root_generate_dictionary(
  PMuppCanvasDict 
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${CMAKE_CURRENT_SOURCE_DIR}/..
  PMuppCanvas.h
  LINKDEF PMuppCanvasLinkDef.h
  MODULE PMuppCanvas
)
root_generate_dictionary(
  PMuppStartupHandlerDict 
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${CMAKE_CURRENT_SOURCE_DIR}/..
  PMuppStartupHandler.h
  LINKDEF PMuppStartupHandlerLinkDef.h
  MODULE PMuppStartupHandler
)

#--- define all the dictonary cxx ---------------------------------------------
set(DICT
  ${PLOT_DIR}/PMuppCanvasDict.cxx
  ${PLOT_DIR}/PMuppStartupHandlerDict.cxx
)

#--- define executable --------------------------------------------------------
add_executable(mupp_plot ${SOURCE} ${DICT})

#--- add all needed include directories ---------------------------------------
target_include_directories(mupp_plot
  PRIVATE
    ${ROOT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

#--- define all to be linked libs ---------------------------------------------
target_link_libraries(mupp_plot ${ROOT_LIBRARIES})

#--- install ------------------------------------------------------------------
if (APPLE)
  install(TARGETS mupp_plot DESTINATION /Applications/mupp.app/Contents/MacOS)
else (APPLE)
  install(TARGETS mupp_plot DESTINATION bin)
endif (APPLE)
