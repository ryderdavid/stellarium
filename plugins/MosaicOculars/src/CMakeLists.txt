# Include Stellarium headers
IF(DEFINED STELLARIUM_SOURCE_DIR)
     SET(STELLARIUM_SRC "${STELLARIUM_SOURCE_DIR}")
ELSE()
     # Assume we're building within Stellarium tree
     SET(STELLARIUM_SRC "${CMAKE_SOURCE_DIR}/../..")
ENDIF()

INCLUDE_DIRECTORIES(
     .
     gui
     ${CMAKE_CURRENT_BINARY_DIR}
     ${CMAKE_CURRENT_BINARY_DIR}/gui
     ${CMAKE_BINARY_DIR}/src
     ${STELLARIUM_SRC}/src
     ${STELLARIUM_SRC}/src/core
     ${STELLARIUM_SRC}/src/core/modules
     ${STELLARIUM_SRC}/src/gui
     ${CMAKE_BINARY_DIR}/plugins/MosaicOculars/src
)

# For dynamic plugins, we don't link against Stellarium libraries
# They're resolved at runtime via -undefined dynamic_lookup

SET(MosaicOculars_SRCS
     CCD.hpp
     CCD.cpp
     Lens.hpp
     Lens.cpp
     Ocular.hpp
     Ocular.cpp
     MosaicOculars.hpp
     MosaicOculars.cpp
     Telescope.hpp
     Telescope.cpp
     gui/MosaicOcularsDialog.hpp
     gui/MosaicOcularsDialog.cpp
     gui/PropertyBasedTableModel.hpp
     gui/PropertyBasedTableModel.cpp
     gui/MosaicOcularsGuiPanel.hpp
     gui/MosaicOcularsGuiPanel.cpp
)

SET(MosaicOculars_RES ../resources/MosaicOculars.qrc)

################# compiles .ui files ############
SET(MosaicOculars_UIS
     gui/mosaicOcularsDialog.ui
)

# Note: The .ui file will be renamed from ocularDialog.ui to mosaicOcularsDialog.ui
# The generated UI header will be ui_mosaicOcularsDialog.h

# Use Qt6 (Qt5 support can be added later if needed)
qt6_wrap_ui(MosaicOculars_UIS_H ${MosaicOculars_UIS})
qt6_add_resources(MosaicOculars_RES_CXX ${MosaicOculars_RES})

# Set output directory for UI files - Qt6 generates them in CMAKE_CURRENT_BINARY_DIR
# The UI header will be at ${CMAKE_CURRENT_BINARY_DIR}/ui_mosaicOcularsDialog.h

SET(MosaicOculars_Qt_Libraries Qt6::Core Qt6::Widgets)

############### For building the dynamic library ######################
# Always build as dynamic plugin
ADD_LIBRARY(MosaicOculars MODULE ${MosaicOculars_SRCS} ${MosaicOculars_RES_CXX} ${MosaicOculars_UIS_H})
# Enable AUTOMOC to process Q_OBJECT and Q_PLUGIN_METADATA macros
SET_TARGET_PROPERTIES(MosaicOculars PROPERTIES AUTOMOC ON)
IF(APPLE)
     FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
     MARK_AS_ADVANCED(OPENGL_LIBRARY)
     # Use @rpath for Qt frameworks so plugin can find Stellarium's bundled Qt
     # -undefined dynamic_lookup allows symbols to be resolved at runtime from Stellarium
     SET_TARGET_PROPERTIES(MosaicOculars PROPERTIES 
          LINK_FLAGS "-undefined dynamic_lookup"
          INSTALL_RPATH "@executable_path/../Frameworks"
          BUILD_WITH_INSTALL_RPATH TRUE
          SUFFIX ".dylib")
ENDIF()

IF(WIN32)
     SET_TARGET_PROPERTIES(MosaicOculars PROPERTIES LINK_FLAGS "-enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition" )
     SET(StelMain stelMain)
ELSE(WIN32)
     SET(StelMain )
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(MosaicOculars ${StelMain} ${MosaicOculars_Qt_Libraries})
INSTALL(TARGETS MosaicOculars DESTINATION "modules/MosaicOculars")

