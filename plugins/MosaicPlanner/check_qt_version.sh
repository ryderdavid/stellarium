#!/bin/bash

# Script to check Qt version used by production Stellarium
# This helps diagnose plugin compatibility issues

echo "Checking Qt version compatibility..."

# Try to find production Stellarium
STELLARIUM_APP=""
if [[ -d "/Applications/Stellarium.app" ]]; then
    STELLARIUM_APP="/Applications/Stellarium.app"
elif [[ -d "$HOME/Applications/Stellarium.app" ]]; then
    STELLARIUM_APP="$HOME/Applications/Stellarium.app"
fi

if [[ -n "$STELLARIUM_APP" ]]; then
    echo "Found Stellarium at: $STELLARIUM_APP"
    echo ""
    echo "Qt libraries used by Stellarium:"
    find "$STELLARIUM_APP" -name "Qt*" -type f 2>/dev/null | head -5
    echo ""
    echo "Checking main executable:"
    EXECUTABLE=$(find "$STELLARIUM_APP/Contents/MacOS" -type f 2>/dev/null | head -1)
    if [[ -n "$EXECUTABLE" ]]; then
        echo "Executable: $EXECUTABLE"
        otool -L "$EXECUTABLE" 2>/dev/null | grep -i qt | head -5
    fi
else
    echo "Production Stellarium not found in standard locations."
    echo ""
    echo "Please provide the path to your Stellarium.app:"
    read -p "Path: " STELLARIUM_APP
    
    if [[ -n "$STELLARIUM_APP" ]] && [[ -d "$STELLARIUM_APP" ]]; then
        EXECUTABLE=$(find "$STELLARIUM_APP/Contents/MacOS" -type f 2>/dev/null | head -1)
        if [[ -n "$EXECUTABLE" ]]; then
            echo "Qt libraries used:"
            otool -L "$EXECUTABLE" 2>/dev/null | grep -i qt | head -5
        fi
    fi
fi

echo ""
echo "Qt version used to build plugin:"
find /opt/homebrew/opt/qt@5 -name "QtCore*" -type f 2>/dev/null | head -1 | xargs otool -L 2>/dev/null | grep -i qtcore | head -1

echo ""
echo "Solution:"
echo "The plugin needs to be built with the same Qt version as production Stellarium."
echo "If production Stellarium uses a bundled Qt, you may need to:"
echo "1. Build Stellarium from source with the same Qt version, OR"
echo "2. Copy Qt frameworks into the plugin bundle (complex), OR"  
echo "3. Use a static build of Stellarium instead"

