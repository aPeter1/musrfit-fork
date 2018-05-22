#--- musredit for Qt > 5.0 ----------------------------------------------------

message("debug> will deal with the build of musredit (Qt 5)")

#--- check if Qt5WebEngine or Qt5WebKit is present ----------------------------
find_package(Qt5WebEngine QUIET)
find_package(Qt5WebKit QUIET)

if (Qt5WebEngine_FOUND)
  message("debug> Qt5WebEngine is present.")
elseif (Qt5WebKit_FOUND)
  message("debug> Qt5WebKit is present.")
else (Qt5WebEngine_FOUND)
  message("debug> Neither Qt5WebEngine nor Qt5WebKit found.")
endif (Qt5WebEngine_FOUND)
