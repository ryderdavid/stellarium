# MosaicPlanner Plugin - Quick Start Guide

## Plugin Name in List

The plugin appears as **"Mosaic Planner"** in the Stellarium plugins list.

## Current Qt Version Situation

### Production Stellarium
- **Version**: 25.2
- **Qt Version**: **6.5.3** (bundled in app)
- **Location**: `/Applications/Stellarium.app/Contents/Frameworks/`

### Plugin Build
- **Current Build**: Qt 6.9.3 (Homebrew)
- **Issue**: Qt plugin loader requires **exact version match** (6.5.3)
- **Status**: Plugin is rejected by Qt version check

### Development Build
- **Uses**: Qt 5.15.17 (matches plugin build)
- **Status**: ✅ Plugin works with development build

## Option 1: Use Development Build (Recommended for Testing)

### Quick Start

```bash
# 1. Build Stellarium with Qt 5
cd /Users/ryder/Documents/GitHub/stellarium/build
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..
make stellarium -j$(sysctl -n hw.ncpu)

# 2. Run development build
./src/stellarium

# 3. Enable plugin
# - Press F2 → Plugins tab
# - Find "Mosaic Planner" in list
# - Click "Load now"
```

### Full Instructions

See `DEV_BUILD_INSTRUCTIONS.md` for detailed instructions.

## Option 2: Get Qt 6.5.3 for Production Stellarium

### Download Qt 6.5.3 from Qt Company

1. **Visit Qt Download Page**:
   - https://www.qt.io/download-qt-installer
   - Or direct: https://www.qt.io/download-qt-installer-oss

2. **Download Qt Online Installer**:
   - Download the macOS installer
   - Run the installer
   - Create a Qt account (free) if needed

3. **Install Qt 6.5.3**:
   - In the installer, select Qt 6.5.3
   - Choose components: Qt 6.5.3 → macOS
   - Install to: `/opt/qt6.5.3` (or your preferred location)

4. **Build Plugin with Qt 6.5.3**:
   ```bash
   cd /Users/ryder/Documents/GitHub/stellarium/build
   cmake -DBUILD_DYNAMIC_PLUGIN=ON \
         -DCMAKE_PREFIX_PATH=/opt/qt6.5.3/6.5.3/macos \
         -DENABLE_QT6=ON ..
   make MosaicPlanner -j$(sysctl -n hw.ncpu)
   
   # Install plugin
   ./../plugins/MosaicPlanner/install_plugin.sh
   ```

5. **Verify Plugin**:
   ```bash
   # Check Qt version in plugin
   strings ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep "Qt 6\."
   ```
   Should show: `Qt 6.5.3`

### Alternative: Use Qt Maintenance Tool

If you already have Qt installed:

1. **Open Qt Maintenance Tool**:
   ```bash
   /path/to/Qt/MaintenanceTool.app
   ```

2. **Add Qt 6.5.3**:
   - Go to "Add or Remove Components"
   - Select Qt 6.5.3
   - Install to desired location

3. **Build Plugin**:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.3/macos ..
   ```

## Option 3: Build Qt 6.5.3 from Source

If you need exact control:

```bash
# Clone Qt repository
git clone https://code.qt.io/qt/qtbase.git
cd qtbase
git checkout v6.5.3

# Build Qt (this takes several hours)
./configure -prefix /opt/qt6.5.3
make -j$(sysctl -n hw.ncpu)
make install
```

**Note**: This is time-consuming (several hours)

## Verification

### Check Plugin Appears

1. Restart Stellarium
2. Go to Configuration → Plugins
3. Look for **"Mosaic Planner"** in the list
4. It should appear alphabetically

### Check Plugin Loads

1. Select "Mosaic Planner" in the list
2. Click "Load now"
3. Check for errors in the info panel

### Test Mosaic Mode

Open scripting console (`~` or `F12`) and run:

```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 3;
MosaicPlanner.mosaicPanelsY = 3;
```

You should see yellow mosaic panel outlines on the sky!

## Troubleshooting

### Plugin Doesn't Appear

1. **Check Plugin File**:
   ```bash
   ls -lh ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/
   ```

2. **Check Log**:
   ```bash
   tail -50 ~/Library/Application\ Support/Stellarium/log.txt | grep -i mosaic
   ```

3. **Common Issues**:
   - Qt version mismatch (plugin not loaded)
   - Plugin file missing or corrupted
   - Wrong Stellarium version (dev vs production)

### Plugin Appears But Doesn't Work

1. **Check Mosaic Mode**:
   ```javascript
   MosaicPlanner.flagMosaicMode  // Should be true
   ```

2. **Check Equipment**:
   - Ensure CCD and Telescope are configured in Oculars
   - MosaicPlanner uses the same equipment

3. **Check Log**:
   ```bash
   tail -100 ~/Library/Application\ Support/Stellarium/log.txt
   ```

## Summary

**For Testing**: Use development build (Qt 5) - works immediately  
**For Production**: Get Qt 6.5.3 from Qt Company and rebuild plugin

**Plugin Name**: "Mosaic Planner"  
**Plugin ID**: `MosaicPlanner`  
**Status**: Works with dev build, needs Qt 6.5.3 for production

