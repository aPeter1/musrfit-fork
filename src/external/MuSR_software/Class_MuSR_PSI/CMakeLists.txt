# - Class_MuSR_PSI library ----------------------------------------------------

#--- create pkg-config info ---------------------------------------------------
set(prefix "${CMAKE_INSTALL_PREFIX}")   
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{exec_prefix\}/lib")
set(includedir "\$\{prefix\}/include")   
set(PSIBIN_VERSION "0.1.0")
set(PSIBIN_LIBRARY_NAME "Class_MuSR_PSI")
configure_file("Class_MuSR_PSI.pc.in" "Class_MuSR_PSI.pc" @ONLY)

#--- lib creation -------------------------------------------------------------
add_library(Class_MuSR_PSI SHARED MuSR_td_PSI_bin.cpp)

#--- install Class_MuSR_PSI solib ---------------------------------------------
install(TARGETS Class_MuSR_PSI DESTINATION lib)

#--- install MuSR_td_PSI_bin header -------------------------------------------
install(FILES MuSR_td_PSI_bin.h DESTINATION include)

#--- install pkg-config info --------------------------------------------------
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/Class_MuSR_PSI.pc
  DESTINATION lib/pkgconfig
)
