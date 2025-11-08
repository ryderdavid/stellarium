set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

ADD_EXECUTABLE(testOcularsMosaic testOcularsMosaic.cpp testOcularsMosaic.hpp)
TARGET_LINK_LIBRARIES(testOcularsMosaic Qt${QT_VERSION_MAJOR}::Test Oculars-static stelMain)
ADD_TEST(testOcularsMosaic testOcularsMosaic)
SET_TARGET_PROPERTIES(testOcularsMosaic PROPERTIES FOLDER "plugins/Oculars/test")

