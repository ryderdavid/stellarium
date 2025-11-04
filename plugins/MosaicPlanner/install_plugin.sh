#!/bin/bash

# Installation script for MosaicPlanner plugin to production Stellarium
# This script copies the built plugin to the user's Stellarium modules directory

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Determine the Stellarium user data directory based on OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    STELLARIUM_DIR="$HOME/Library/Application Support/Stellarium"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    STELLARIUM_DIR="$HOME/.stellarium"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    # Windows (Git Bash/Cygwin)
    STELLARIUM_DIR="$APPDATA/Stellarium"
else
    echo -e "${RED}Error: Unsupported operating system${NC}"
    exit 1
fi

PLUGIN_DIR="$STELLARIUM_DIR/modules/MosaicPlanner"
BUILD_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../build/plugins/MosaicPlanner/src" && pwd)"
PLUGIN_LIB="libMosaicPlanner.dylib"

# Check if we're in the right directory (should be in stellarium repo root)
if [ ! -d "plugins/MosaicPlanner" ]; then
    echo -e "${RED}Error: Must be run from Stellarium repository root${NC}"
    exit 1
fi

# Check if the plugin was built
if [ ! -f "$BUILD_DIR/$PLUGIN_LIB" ]; then
    # Try alternative paths for different OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PLUGIN_LIB="libMosaicPlanner.so"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        PLUGIN_LIB="MosaicPlanner.dll"
    fi
    
    if [ ! -f "$BUILD_DIR/$PLUGIN_LIB" ]; then
        echo -e "${RED}Error: Plugin not found at $BUILD_DIR/$PLUGIN_LIB${NC}"
        echo -e "${YELLOW}Please build the plugin first:${NC}"
        echo "  cd build && cmake -DBUILD_DYNAMIC_PLUGIN=ON .. && make MosaicPlanner"
        exit 1
    fi
fi

echo -e "${GREEN}Installing MosaicPlanner plugin...${NC}"

# Create plugin directory if it doesn't exist
mkdir -p "$PLUGIN_DIR"

# Copy the plugin library
echo -e "Copying plugin to: ${YELLOW}$PLUGIN_DIR${NC}"
cp "$BUILD_DIR/$PLUGIN_LIB" "$PLUGIN_DIR/"

# Fix Qt library paths on macOS to use Stellarium's bundled Qt
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLUGIN_FILE="$PLUGIN_DIR/$PLUGIN_LIB"
    echo -e "Fixing Qt library paths for macOS..."
    
    # Check if Qt frameworks exist in Stellarium
    STELLARIUM_APP=""
    if [[ -d "/Applications/Stellarium.app" ]]; then
        STELLARIUM_APP="/Applications/Stellarium.app"
    elif [[ -d "$HOME/Applications/Stellarium.app" ]]; then
        STELLARIUM_APP="$HOME/Applications/Stellarium.app"
    fi
    
    if [[ -n "$STELLARIUM_APP" ]] && [[ -d "$STELLARIUM_APP/Contents/Frameworks/QtCore.framework" ]]; then
        # Change Qt library paths to use Stellarium's bundled Qt
        install_name_tool -change /opt/homebrew/opt/qtbase/lib/QtCore.framework/Versions/A/QtCore \
            @executable_path/../Frameworks/QtCore.framework/Versions/A/QtCore "$PLUGIN_FILE" 2>/dev/null
        
        install_name_tool -change /opt/homebrew/opt/qtbase/lib/QtGui.framework/Versions/A/QtGui \
            @executable_path/../Frameworks/QtGui.framework/Versions/A/QtGui "$PLUGIN_FILE" 2>/dev/null
        
        install_name_tool -change /opt/homebrew/opt/qtbase/lib/QtWidgets.framework/Versions/A/QtWidgets \
            @executable_path/../Frameworks/QtWidgets.framework/Versions/A/QtWidgets "$PLUGIN_FILE" 2>/dev/null
        
        # Also handle @rpath paths if they exist
        install_name_tool -change @rpath/QtCore.framework/Versions/A/QtCore \
            @executable_path/../Frameworks/QtCore.framework/Versions/A/QtCore "$PLUGIN_FILE" 2>/dev/null
        
        install_name_tool -change @rpath/QtGui.framework/Versions/A/QtGui \
            @executable_path/../Frameworks/QtGui.framework/Versions/A/QtGui "$PLUGIN_FILE" 2>/dev/null
        
        install_name_tool -change @rpath/QtWidgets.framework/Versions/A/QtWidgets \
            @executable_path/../Frameworks/QtWidgets.framework/Versions/A/QtWidgets "$PLUGIN_FILE" 2>/dev/null
        
        echo -e "${GREEN}✓ Qt library paths fixed${NC}"
    else
        echo -e "${YELLOW}Warning: Could not find Stellarium.app - Qt paths not fixed${NC}"
    fi
fi

# Verify the copy was successful
if [ -f "$PLUGIN_DIR/$PLUGIN_LIB" ]; then
    echo -e "${GREEN}✓ Plugin installed successfully!${NC}"
    echo ""
    echo "Plugin location: $PLUGIN_DIR/$PLUGIN_LIB"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. Restart Stellarium if it's running"
    echo "2. The plugin should appear in the plugins list"
    echo "3. Enable the MosaicPlanner plugin"
    echo "4. The plugin will load equipment from modules/Oculars/ocular.ini"
else
    echo -e "${RED}Error: Installation failed - plugin file not found after copy${NC}"
    exit 1
fi

