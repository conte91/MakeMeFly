FIND_PATH(Stellarisware_INCLUDE_DIRS NAMES "inc/hw_gpio.h" PATHS "${STELLARISWARE_ROOT}" DOC "The Stellaris driver include directory" NO_DEFAULT_PATH)
FIND_LIBRARY(Stellarisware_LIBRARIES NAMES driver-cm4f driver PATHS "${STELLARISWARE_ROOT}/driverlib/gcc-cm4f" "${STELLARISWARE_ROOT}/driverlib/gcc" DOC "The Stellaris driver include directory" NO_DEFAULT_PATH)

SET(STELLARISWARE_FOUND FALSE)

IF(STELLARISWARE_INCLUDE_DIR AND STELLARISWARE_LIBRARY)
  SET(STELLARISWARE_FOUND TRUE)
ELSE()
  MESSAGE(WARNING "Couldn't find Stellarisware libraries. Have you set STELLARISWARE_ROOT to the root of the stellarisware lib? Try again with cmake -DSTELLARISWARE_ROOT=/path/to/stellarisware :)")
ENDIF()

#Marks as advanced variables the paths, so the gui-user will not modify it (but who cares? Who uses GUIs anymore nowadays?)
MARK_AS_ADVANCED(STELLARISWARE_INCLUDE_DIR STELLARISWARE_LIBRARY)
