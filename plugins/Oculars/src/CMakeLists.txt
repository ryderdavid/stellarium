INCLUDE_DIRECTORIES(
     .
     gui
     ${CMAKE_BINARY_DIR}/plugins/Oculars/src
)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)
IF(DEFINED BUILD_DIR)
     LINK_DIRECTORIES(${BUILD_DIR}/src)
ENDIF()

SET(Oculars_SRCS
     CCD.hpp
     CCD.cpp
     Lens.hpp
     Lens.cpp
     Ocular.hpp
     Ocular.cpp
     Oculars.hpp
     Oculars.cpp
     Telescope.hpp
     Telescope.cpp
     gui/OcularDialog.hpp
     gui/OcularDialog.cpp
     gui/PropertyBasedTableModel.hpp
     gui/PropertyBasedTableModel.cpp
     gui/OcularsGuiPanel.hpp
     gui/OcularsGuiPanel.cpp
)

SET(Oculars_RES ../resources/Oculars.qrc)

################# compiles .ui files ############
SET(Oculars_UIS
     gui/ocularDialog.ui
)

IF (${QT_VERSION_MAJOR} EQUAL "5")
      QT5_WRAP_UI(Oculars_UIS_H ${Oculars_UIS})
      QT5_ADD_RESOURCES(Oculars_RES_CXX ${Oculars_RES})
ELSE()
     QT_WRAP_UI(Oculars_UIS_H ${Oculars_UIS})
     QT_ADD_RESOURCES(Oculars_RES_CXX ${Oculars_RES})
ENDIF()

SET(Oculars_Qt_Libraries Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

############### For building the dynamic library ######################
IF(BUILD_DYNAMIC_PLUGIN)
     ADD_LIBRARY(Oculars MODULE ${Oculars_SRCS} ${Oculars_RES_CXX} ${Oculars_UIS_H})
     IF(APPLE)
          FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
          MARK_AS_ADVANCED(OPENGL_LIBRARY)
          SET_TARGET_PROPERTIES(Oculars PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" SUFFIX ".dylib")
     ENDIF()

     IF(WIN32)
          SET_TARGET_PROPERTIES(Oculars PROPERTIES LINK_FLAGS "-enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition" )
          SET(StelMain stelMain)
     ELSE(WIN32)
          SET(StelMain )
     ENDIF(WIN32)

     TARGET_LINK_LIBRARIES(Oculars ${StelMain} ${Oculars_Qt_Libraries})
     INSTALL(TARGETS Oculars DESTINATION "modules/Oculars")
ELSE()
############### For building the static library ######################
     ADD_LIBRARY(Oculars-static STATIC ${Oculars_SRCS} ${Oculars_RES_CXX} ${Oculars_UIS_H})
     SET_TARGET_PROPERTIES(Oculars-static PROPERTIES OUTPUT_NAME "Oculars")
     TARGET_LINK_LIBRARIES(Oculars-static ${Oculars_Qt_Libraries})
     SET_TARGET_PROPERTIES(Oculars-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
     ADD_DEPENDENCIES(AllStaticPlugins Oculars-static)
     SET_TARGET_PROPERTIES(Oculars-static PROPERTIES FOLDER "plugins/Oculars")
ENDIF()
