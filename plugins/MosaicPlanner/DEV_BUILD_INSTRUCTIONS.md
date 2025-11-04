# Using MosaicPlanner with Development Build

## Overview

The MosaicPlanner plugin works with the development build of Stellarium from this repository. The development build uses Qt 5, which matches the plugin build.

## Step-by-Step Instructions

### Step 1: Build Stellarium (Development Version)

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build

# Clean previous build if needed
rm -rf *  # or just reconfigure

# Configure with Qt 5
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..

# Build Stellarium
make stellarium -j$(sysctl -n hw.ncpu)
```

### Step 2: Run Development Build

```bash
# From the build directory
./src/stellarium

# Or from the repository root
./build/src/stellarium
```

### Step 3: Verify Plugin is Installed

The plugin should already be installed at:
```
~/Library/Application Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib
```

Verify it exists:
```bash
ls -lh ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/
```

### Step 4: Enable Plugin in Stellarium

1. **Open Stellarium** (development build)
2. **Open Configuration**: Press `F2` or go to **Configuration** → **Plugins**
3. **Find Plugin**: Look for **"Mosaic Planner"** in the plugins list
   - It should appear in alphabetical order
   - Click on it to select
4. **Enable Plugin**: 
   - Check **"Load at startup"** if you want it to load automatically
   - Click **"Load now"** to enable it immediately
5. **Verify**: The plugin should now be loaded

### Step 5: Test Mosaic Mode

Open the **Scripting Console** (press `~` or `F12`) and run:

```javascript
// Enable mosaic mode
MosaicPlanner.flagMosaicMode = true;

// Configure a 3x3 mosaic with 20% overlap
MosaicPlanner.mosaicPanelsX = 3;
MosaicPlanner.mosaicPanelsY = 3;
MosaicPlanner.mosaicOverlapPercent = 20.0;
MosaicPlanner.mosaicRotationAngle = 0.0;
```

You should see yellow mosaic panel outlines appear on the sky!

## Plugin Properties (Scripting API)

All properties are accessible via scripting console:

```javascript
// Enable/disable mosaic mode
MosaicPlanner.flagMosaicMode = true;

// Configure panel count (1-20)
MosaicPlanner.mosaicPanelsX = 5;
MosaicPlanner.mosaicPanelsY = 3;

// Configure rotation (0-360 degrees)
MosaicPlanner.mosaicRotationAngle = 45.0;

// Configure overlap (0-50 percent)
MosaicPlanner.mosaicOverlapPercent = 25.0;
```

## Equipment Configuration

The plugin automatically loads equipment from the **Oculars plugin** configuration:

**Location**: `~/Library/Application Support/Stellarium/modules/Oculars/ocular.ini`

**To Configure Equipment**:

1. **Open Oculars Plugin Configuration**:
   - Configuration → Plugins → Oculars → Configure
2. **Add Equipment**:
   - Add CCD (camera sensor)
   - Add Telescope
   - Add Lens (optional - reducer/Barlow)
3. **MosaicPlanner Uses Same Equipment**:
   - MosaicPlanner automatically uses the same equipment
   - No need to configure separately

## Troubleshooting

### Plugin Doesn't Appear in List

1. **Check Plugin File Exists**:
   ```bash
   ls -lh ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib
   ```

2. **Check Stellarium Log**:
   ```bash
   tail -100 ~/Library/Application\ Support/Stellarium/log.txt | grep -i mosaic
   ```

3. **Verify You're Running Dev Build**:
   - Make sure you're running `./build/src/stellarium`
   - Not `/Applications/Stellarium.app` (production)

4. **Rebuild Plugin if Needed**:
   ```bash
   cd build
   make MosaicPlanner
   ./../plugins/MosaicPlanner/install_plugin.sh
   ```

### Mosaic Not Displaying

1. **Check Mosaic Mode is Enabled**:
   ```javascript
   MosaicPlanner.flagMosaicMode  // Should be true
   ```

2. **Verify Equipment is Configured**:
   - Open Oculars plugin configuration
   - Ensure at least one CCD and Telescope are configured
   - MosaicPlanner uses the first CCD and Telescope by default

3. **Check Equipment Selection**:
   ```javascript
   // The plugin uses equipment indices from Oculars
   // Make sure equipment is configured in Oculars first
   ```

### Plugin Crashes or Errors

1. **Check Log File**:
   ```bash
   tail -50 ~/Library/Application\ Support/Stellarium/log.txt
   ```

2. **Verify Qt Compatibility**:
   - Development build uses Qt 5
   - Plugin built with Qt 5
   - Should be compatible

3. **Rebuild Plugin**:
   ```bash
   cd build
   make MosaicPlanner
   ./../plugins/MosaicPlanner/install_plugin.sh
   ```

## Rebuilding After Changes

If you make changes to the plugin code:

```bash
cd build
make MosaicPlanner
./../plugins/MosaicPlanner/install_plugin.sh
```

Then restart Stellarium (development build).

## Advantages of Development Build

- ✅ **Plugin Works Immediately**: Qt 5 compatibility
- ✅ **Easy Testing**: Can test and debug quickly
- ✅ **Full Development Access**: Can modify and rebuild
- ✅ **No Version Conflicts**: Qt 5 matches plugin build

## Disadvantages

- ⚠️ **Not Production**: Different from production Stellarium
- ⚠️ **May Have Bugs**: Development version may have issues
- ⚠️ **Requires Building**: Must build from source

## Quick Reference

**Plugin Name in List**: "Mosaic Planner"

**Plugin ID**: `MosaicPlanner`

**Scripting Access**: `MosaicPlanner.propertyName`

**Settings Location**: `~/Library/Application Support/Stellarium/modules/MosaicPlanner/mosaicplanner.ini`

**Equipment Source**: `~/Library/Application Support/Stellarium/modules/Oculars/ocular.ini`
