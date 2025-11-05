cmake_minimum_required(VERSION 3.10)
project(MosaicOculars VERSION 1.0.0)

SET(MOSAICOCULARS_VERSION "1.0.0")

# Find Qt
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

ADD_SUBDIRECTORY( src )
IF(APPLE)
     SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/Library/Application\ Support/Stellarium)
ELSE(APPLE)
     SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/.stellarium)
ENDIF(APPLE)
INSTALL(FILES DESTINATION "modules/MosaicOculars")

