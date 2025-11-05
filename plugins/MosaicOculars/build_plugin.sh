#!/bin/bash
# Build script for MosaicOculars dynamic plugin
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
PLUGIN_NAME="MosaicOculars"

echo "Building $PLUGIN_NAME dynamic plugin..."

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    PLUGIN_LIB="lib${PLUGIN_NAME}.dylib"
    STELLARIUM_USER_DATA="$HOME/Library/Application Support/Stellarium"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    PLUGIN_LIB="lib${PLUGIN_NAME}.so"
    STELLARIUM_USER_DATA="$HOME/.stellarium"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    OS="windows"
    PLUGIN_LIB="${PLUGIN_NAME}.dll"
    STELLARIUM_USER_DATA="$APPDATA/Stellarium"
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

PLUGIN_DIR="$STELLARIUM_USER_DATA/modules/$PLUGIN_NAME"

# Find Qt 6.5.3 to match Stellarium's version (prefer Qt 6.5.3)
QT_PREFIX=""
if [[ -d "$HOME/Qt/6.5.3/macos" ]]; then
    QT_PREFIX="$HOME/Qt/6.5.3/macos"
    echo "Using Qt 6.5.3 from: $QT_PREFIX"
elif command -v qmake6 &> /dev/null; then
    QT_PREFIX=$(qmake6 -query QT_INSTALL_PREFIX)
    echo "Using Qt from qmake6: $QT_PREFIX"
elif command -v qmake &> /dev/null; then
    QT_PREFIX=$(qmake -query QT_INSTALL_PREFIX)
    echo "Using Qt from qmake: $QT_PREFIX"
elif [[ -d "/opt/homebrew/opt/qt" ]]; then
    QT_PREFIX="/opt/homebrew/opt/qt"
    echo "Using Qt from Homebrew: $QT_PREFIX"
fi

if [[ -z "$QT_PREFIX" ]]; then
    echo "Error: Could not find Qt installation"
    exit 1
fi

echo "Using Qt at: $QT_PREFIX"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Set Stellarium source directory (parent of plugins directory)
STELLARIUM_SRC="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Configure CMake
cmake -DBUILD_DYNAMIC_PLUGIN=ON \
      -DCMAKE_PREFIX_PATH="$QT_PREFIX" \
      -DENABLE_QT6=ON \
      -DSTELLARIUM_SOURCE_DIR="$STELLARIUM_SRC" \
      "$SCRIPT_DIR"

# Build
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# Find the built library (it's in build/src/ directory)
if [[ "$OS" == "macos" ]]; then
    PLUGIN_LIB="libMosaicOculars.dylib"
    PLUGIN_BUILD_PATH="src/$PLUGIN_LIB"
elif [[ "$OS" == "linux" ]]; then
    PLUGIN_LIB="libMosaicOculars.so"
    PLUGIN_BUILD_PATH="src/$PLUGIN_LIB"
else
    PLUGIN_LIB="MosaicOculars.dll"
    PLUGIN_BUILD_PATH="src/$PLUGIN_LIB"
fi

# Install
mkdir -p "$PLUGIN_DIR"
if [[ -f "$PLUGIN_BUILD_PATH" ]]; then
    cp "$PLUGIN_BUILD_PATH" "$PLUGIN_DIR/"
    INSTALLED_FILE="$PLUGIN_DIR/$PLUGIN_LIB"
    echo "✓ Plugin library copied to: $INSTALLED_FILE"
elif [[ -f "$PLUGIN_LIB" ]]; then
    # Try in current directory
    cp "$PLUGIN_LIB" "$PLUGIN_DIR/"
    INSTALLED_FILE="$PLUGIN_DIR/$PLUGIN_LIB"
    echo "✓ Plugin library copied to: $INSTALLED_FILE"
else
    echo "Error: Could not find built library $PLUGIN_LIB"
    echo "Searched in: $PLUGIN_BUILD_PATH and $PLUGIN_LIB"
    echo "Build directory contents:"
    ls -la src/ 2>/dev/null || ls -la . 2>/dev/null
    exit 1
fi

# Fix library paths on macOS
if [[ "$OS" == "macos" ]]; then
    echo "Fixing Qt library paths for macOS..."
    
    # Check if Stellarium.app exists
    STELLARIUM_APP=""
    if [[ -d "/Applications/Stellarium.app" ]]; then
        STELLARIUM_APP="/Applications/Stellarium.app"
    elif [[ -d "$HOME/Applications/Stellarium.app" ]]; then
        STELLARIUM_APP="$HOME/Applications/Stellarium.app"
    fi
    
    if [[ -n "$STELLARIUM_APP" ]] && [[ -d "$STELLARIUM_APP/Contents/Frameworks/QtCore.framework" ]]; then
        # Change Qt library paths to use Stellarium's bundled Qt
        # Try various possible Qt paths that might be in the library
        for QT_LIB_PATH in \
            "$QT_PREFIX/lib/QtCore.framework/Versions/A/QtCore" \
            "$QT_PREFIX/opt/qtbase/lib/QtCore.framework/Versions/A/QtCore" \
            "/opt/homebrew/opt/qtbase/lib/QtCore.framework/Versions/A/QtCore" \
            "/opt/homebrew/lib/QtCore.framework/Versions/A/QtCore" \
            "@rpath/QtCore.framework/Versions/A/QtCore"; do
            install_name_tool -change "$QT_LIB_PATH" \
                @executable_path/../Frameworks/QtCore.framework/Versions/A/QtCore "$INSTALLED_FILE" 2>/dev/null || true
        done
        
        for QT_LIB_PATH in \
            "$QT_PREFIX/lib/QtGui.framework/Versions/A/QtGui" \
            "$QT_PREFIX/opt/qtbase/lib/QtGui.framework/Versions/A/QtGui" \
            "/opt/homebrew/opt/qtbase/lib/QtGui.framework/Versions/A/QtGui" \
            "/opt/homebrew/lib/QtGui.framework/Versions/A/QtGui" \
            "@rpath/QtGui.framework/Versions/A/QtGui"; do
            install_name_tool -change "$QT_LIB_PATH" \
                @executable_path/../Frameworks/QtGui.framework/Versions/A/QtGui "$INSTALLED_FILE" 2>/dev/null || true
        done
        
        for QT_LIB_PATH in \
            "$QT_PREFIX/lib/QtWidgets.framework/Versions/A/QtWidgets" \
            "$QT_PREFIX/opt/qtbase/lib/QtWidgets.framework/Versions/A/QtWidgets" \
            "/opt/homebrew/opt/qtbase/lib/QtWidgets.framework/Versions/A/QtWidgets" \
            "/opt/homebrew/lib/QtWidgets.framework/Versions/A/QtWidgets" \
            "@rpath/QtWidgets.framework/Versions/A/QtWidgets"; do
            install_name_tool -change "$QT_LIB_PATH" \
                @executable_path/../Frameworks/QtWidgets.framework/Versions/A/QtWidgets "$INSTALLED_FILE" 2>/dev/null || true
        done
        
        echo "✓ Qt library paths fixed"
    else
        echo "Warning: Could not find Stellarium.app - Qt paths not fixed"
    fi
fi

echo ""
echo "Build complete! Plugin installed to: $PLUGIN_DIR"
echo "Restart Stellarium and enable the plugin in Configuration > Plugins"
