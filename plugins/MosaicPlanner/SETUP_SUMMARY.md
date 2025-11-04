# MosaicPlanner Plugin - Setup Summary

## Plugin Name

**"Mosaic Planner"** - This is what you'll see in the Stellarium plugins list.

## Current Situation

### Production Stellarium (25.2)
- **Qt Version**: **6.5.3** (bundled in app)
- **Plugin Status**: ❌ Cannot load (version mismatch)

### Development Build
- **Qt Version**: 5.15.17 (matches plugin build)
- **Plugin Status**: ✅ Works immediately

## Solution 1: Use Development Build (Easiest)

### Quick Commands

```bash
# Build Stellarium with Qt 5
cd /Users/ryder/Documents/GitHub/stellarium/build
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..
make stellarium -j$(sysctl -n hw.ncpu)

# Run development build
./src/stellarium
```

### Enable Plugin

1. Press `F2` → Plugins tab
2. Find **"Mosaic Planner"** in the list
3. Click "Load now"

### Test Mosaic Mode

Open scripting console (`~` or `F12`):

```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 3;
MosaicPlanner.mosaicPanelsY = 3;
```

**Full Instructions**: See `DEV_BUILD_INSTRUCTIONS.md`

## Solution 2: Get Qt 6.5.3 for Production Stellarium

### Download Qt 6.5.3 from Qt Company

1. **Visit**: https://www.qt.io/download-qt-installer
2. **Download**: Qt Online Installer for macOS
3. **Install**: Run installer, select Qt 6.5.3
4. **Install Location**: `/opt/qt6.5.3` (or your preference)

### Build Plugin with Qt 6.5.3

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build
cmake -DBUILD_DYNAMIC_PLUGIN=ON \
      -DCMAKE_PREFIX_PATH=/opt/qt6.5.3/6.5.3/macos \
      -DENABLE_QT6=ON ..
make MosaicPlanner -j$(sysctl -n hw.ncpu)
./../plugins/MosaicPlanner/install_plugin.sh
```

### Verify Qt Version

```bash
strings ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep "Qt 6\."
```

Should show: `Qt 6.5.3`

**Full Instructions**: See `QT_VERSION_MATCH.md`

## What You'll See

### In Plugins List
- **Name**: "Mosaic Planner"
- **Location**: Configuration → Plugins tab
- **Appearance**: Alphabetically sorted list

### When Enabled
- Plugin loads without errors
- Properties accessible via scripting
- Mosaic mode can be enabled

### Mosaic Display
- Yellow panel outlines on sky
- Grid of panels based on configuration
- Updates when settings change

## Quick Reference

**Plugin Name**: "Mosaic Planner"  
**Plugin ID**: `MosaicPlanner`  
**Settings File**: `~/Library/Application Support/Stellarium/modules/MosaicPlanner/mosaicplanner.ini`  
**Equipment Source**: `~/Library/Application Support/Stellarium/modules/Oculars/ocular.ini`

## Recommended Approach

**For Testing Now**: Use development build (Solution 1) - works immediately  
**For Production Later**: Get Qt 6.5.3 (Solution 2) - matches production Stellarium

## Files Created

- `DEV_BUILD_INSTRUCTIONS.md` - Full instructions for development build
- `QT_VERSION_MATCH.md` - How to get Qt 6.5.3
- `QUICK_START.md` - Quick reference guide
- `SETUP_SUMMARY.md` - This file (overview)

