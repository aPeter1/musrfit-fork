# - DummyUserFcn library --------------------------------------------------------

#--- generate necessary dictionaries ------------------------------------------
set(MUSRFIT_INC ${CMAKE_SOURCE_DIR}/src/include)
set(DUMMY_USER_FUNC_INC ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
# ROOT requires that the dictonary header files are found at configuration time.
# Hence, target_include_directories cannot be used here because, targets are 
# setup only afterwards.
include_directories(${DUMMY_USER_FUNC_INC})

root_generate_dictionary(
  PDummyUserFcnDict 
  PDummyUserFcn.h 
  OPTIONS 
    -I${MUSRFIT_INC}
    -I${DUMMY_USER_FUNC_INC}
    -inlineInputHeader
  LINKDEF ${DUMMY_USER_FUNC_INC}/PDummyUserFcnLinkDef.h
  MODULE PDummyUserFcn
)

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(P_DUMMY_USER_FUNC_VERSION "1.0.0")
set(P_DUMMY_USER_FUNC_LIBRARY_NAME "PDummyUserFcn")
configure_file("PDummyUserFcn.pc.in" "PDummyUserFcn.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(PDummyUserFcn SHARED
  PDummyUserFcn.cpp
  PDummyUserFcnDict.cxx
)

#--- set target properties, e.g. version --------------------------------------
set_target_properties(PDummyUserFcn
  PROPERTIES
    VERSION ${P_DUMMY_USER_FUNC_VERSION}
)

#--- make sure that the include directory is found ----------------------------
target_include_directories(
  PDummyUserFcn BEFORE PRIVATE 
    $<BUILD_INTERFACE:${MUSRFIT_INC}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../inc>
)

#--- add library dependencies -------------------------------------------------
target_link_libraries(PDummyUserFcn ${ROOT_LIBRARIES} PUserFcnBase)

#--- install PDummyUserFcn solib -------------------------------------------------
install(TARGETS PDummyUserFcn DESTINATION lib)

#--- install root pcm's and rootmaps ------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libPDummyUserFcn_rdict.pcm 
        ${CMAKE_CURRENT_BINARY_DIR}/libPDummyUserFcn.rootmap
  DESTINATION lib
)

#--- install PDummyUserFcn header ------------------------------------------------
install(
  FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/PDummyUserFcn.h 
  DESTINATION 
    include
)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/PDummyUserFcn.pc
  DESTINATION lib/pkgconfig
)

