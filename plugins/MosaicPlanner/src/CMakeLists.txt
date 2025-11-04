INCLUDE_DIRECTORIES(.)

# For dynamic plugins, we don't link against Stellarium libraries
# They're resolved at runtime via -undefined dynamic_lookup
IF(DEFINED BUILD_DIR)
     LINK_DIRECTORIES(${BUILD_DIR}/src)
ELSE()
     LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)
ENDIF()

SET(MosaicPlanner_SRCS
     MosaicPlanner.hpp
     MosaicPlanner.cpp
     CCD.hpp
     CCD.cpp
     Telescope.hpp
     Telescope.cpp
     Lens.hpp
     Lens.cpp
)

SET(MosaicPlanner_Qt_Libraries Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

############### For building the dynamic library ######################
IF(BUILD_DYNAMIC_PLUGIN)
     ADD_LIBRARY(MosaicPlanner MODULE ${MosaicPlanner_SRCS})
     IF(APPLE)
          FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
          MARK_AS_ADVANCED(OPENGL_LIBRARY)
          # Use @rpath for Qt frameworks so plugin can find Stellarium's bundled Qt
          # -undefined dynamic_lookup allows symbols to be resolved at runtime from Stellarium
          SET_TARGET_PROPERTIES(MosaicPlanner PROPERTIES 
               LINK_FLAGS "-undefined dynamic_lookup"
               INSTALL_RPATH "@executable_path/../Frameworks"
               BUILD_WITH_INSTALL_RPATH TRUE
               SUFFIX ".dylib")
     ENDIF()

     IF(WIN32)
          SET_TARGET_PROPERTIES(MosaicPlanner PROPERTIES LINK_FLAGS "-enable-runtime-pseudo-reloc -Wl,--allow-multiple-definition" )
          SET(StelMain stelMain)
     ELSE(WIN32)
          SET(StelMain )
     ENDIF(WIN32)

     TARGET_LINK_LIBRARIES(MosaicPlanner ${StelMain} ${MosaicPlanner_Qt_Libraries})
     INSTALL(TARGETS MosaicPlanner DESTINATION "modules/MosaicPlanner")
ELSE()
############### For building the static library ######################
     ADD_LIBRARY(MosaicPlanner-static STATIC ${MosaicPlanner_SRCS})
     SET_TARGET_PROPERTIES(MosaicPlanner-static PROPERTIES OUTPUT_NAME "MosaicPlanner")
     TARGET_LINK_LIBRARIES(MosaicPlanner-static ${MosaicPlanner_Qt_Libraries})
     SET_TARGET_PROPERTIES(MosaicPlanner-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
     ADD_DEPENDENCIES(AllStaticPlugins MosaicPlanner-static)
     SET_TARGET_PROPERTIES(MosaicPlanner-static PROPERTIES FOLDER "plugins/MosaicPlanner")
ENDIF()

