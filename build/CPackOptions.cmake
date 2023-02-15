# musrfit - cpack

include(CPackComponent)

set (CPACK_PACKAGE_NAME "musrfit-root6")
set (CPACK_PACKAGE_VENDOR "Paul Scherrer Institute - LMU - LEM")
set (CPACK_PACKAGE_DIRECTOR "/Users/maria/Applications/musrfit/build/pack")
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "7")
set (CPACK_PACKAGE_VERSION_PATCH "6")
set (CPACK_PACKAGE_CONTACT "<andreas.suter@psi.ch>")
set (CPACK_PACKAGE_DESCRIPTION_FILE "/Users/maria/Applications/musrfit/README.md")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "musrfit - muSR data analysis package")
set (CPACK_PACKAGE_HOMEPAGE_URL "http://lmu.web.psi.ch/musrfit/user/html/index.html")
set (CPACK_PACKAGE_CHECKSUM "MD5")
set (CPACK_RESOURCE_FILE_LICENSE "/Users/maria/Applications/musrfit/COPYING")
set (CPACK_RESOURCE_FILE_README "/Users/maria/Applications/musrfit/README.md")
set (CPACK_RESOURCE_FILE_WELCOME "/Users/maria/Applications/musrfit/cmake/welcome.txt")

set (CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/local;/usr/local/bin;/usr/local/include;/usr/local/share;/usr/local/lib64;/usr/local/lib;/usr/local/pkgconfig")

# we do not have any absolute paths, so do not need DESTDIR
set (CPACK_SET_DESTDIR "OFF")
set (CPACK_PACKAGE_RELOCATABLE "true")

if (${CPACK_GENERATOR} STREQUAL "RPM")
  set (CPACK_RPM_PACKAGE_REQUIRES "root >= 6.22.00") 
  set (CPACK_INSTALL_PREFIX "/usr/local")
  set (CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
  set (CPACK_RPM_FILE_NAME "RPM-DEFAULT")
  set (CPACK_RPM_PACKAGE_RELEASE "1")
  # next variable for RHEL, CentOS, Fedora
  set (CPACK_RPM_PACKAGE_RELEASE_DIST "")
  set (CPACK_RPM_PACKAGE_LICENSE "GPLv2")
  set (CPACK_RPM_PACKAGE_VENDOR "Paul Scherrer Institute - LMU - LEM")
  set (CPACK_RPM_CHANGELOG_FILE "/Users/maria/Applications/musrfit/cmake/ChangeLogRPM")
  set (CPACK_RPM_POST_INSTALL_SCRIPT_FILE "/Users/maria/Applications/musrfit/cmake/post_install.sh")
  set (CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "/Users/maria/Applications/musrfit/cmake/post_uninstall.sh")
endif ()

if (${CPACK_GENERATOR} STREQUAL "DEB")
  # to be populated yet
endif ()
