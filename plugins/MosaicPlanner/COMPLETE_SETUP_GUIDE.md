# MosaicPlanner Plugin - Complete Setup Guide

## Plugin Name

**"Mosaic Planner"** - This appears in Stellarium's plugins list (Configuration → Plugins tab)

## Qt Version Requirements

### Production Stellarium (25.2)
- **Qt Version**: **6.5.3** (bundled in `/Applications/Stellarium.app`)
- **Source**: Verified from production app's QtCore framework

### Development Build
- **Qt Version**: 5.15.17 (matches plugin build)
- **Status**: ✅ Plugin works immediately

### Current Plugin Build
- **Qt Version**: 6.9.3 (Homebrew)
- **Issue**: Qt plugin loader requires **exact version match** (6.5.3)
- **Error**: `"The plugin uses incompatible Qt library. (6.9.0) [release]"`

---

## Option 1: Use Development Build (EASIEST - Works Now)

### Step 1: Build Stellarium with Qt 5

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build

# Clean previous build (optional)
rm -rf CMakeCache.txt CMakeFiles/

# Configure with Qt 5
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..

# Build Stellarium
make stellarium -j$(sysctl -n hw.ncpu)
```

**Expected Output**: Build completes successfully with Qt 5

### Step 2: Run Development Build

```bash
# From build directory
./src/stellarium
```

### Step 3: Enable Plugin

1. **Open Configuration**: Press `F2` or go to **Configuration** → **Plugins**
2. **Find Plugin**: Look for **"Mosaic Planner"** in the plugins list
3. **Enable**: Click on it, then click **"Load now"**
4. **Verify**: Plugin should load without errors

### Step 4: Test Mosaic Mode

Open scripting console (`~` or `F12`) and run:

```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 3;
MosaicPlanner.mosaicPanelsY = 3;
MosaicPlanner.mosaicOverlapPercent = 20.0;
```

**Result**: Yellow mosaic panel outlines should appear on the sky!

---

## Option 2: Get Qt 6.5.3 for Production Stellarium

### Step 1: Download Qt 6.5.3

Qt 6.5.3 is **not available via Homebrew**. You must download it from Qt Company:

1. **Visit Qt Download Page**:
   - **Main Page**: https://www.qt.io/download-qt-installer
   - **Direct Link**: https://www.qt.io/download-qt-installer-oss

2. **Download Qt Online Installer**:
   - Click "Download" for macOS
   - You'll need to create a free Qt account (if you don't have one)
   - Download the installer (`.dmg` file)

3. **Install Qt Online Installer**:
   ```bash
   # Open the downloaded .dmg
   # Run the installer
   open ~/Downloads/qt-unified-macOS-x64-online.dmg  # (or similar filename)
   ```

### Step 2: Install Qt 6.5.3

1. **Run Qt Maintenance Tool**:
   - Open the Qt installer application
   - Sign in with your Qt account

2. **Add Qt 6.5.3**:
   - Go to **"Add or Remove Components"**
   - Expand **Qt** → **Qt 6.5.3**
   - Select **macOS** (or **macOS (Apple Silicon)** for M1/M2)
   - Also select:
     - Qt 6.5.3 → Sources
     - Qt 6.5.3 → Qt Design Studio
     - Developer and Designer Tools → Qt Creator (optional but recommended)

3. **Choose Installation Path**:
   - Default: `~/Qt/6.5.3/`
   - Recommended: `/opt/qt6.5.3/` (requires sudo)
   - Or use: `~/Qt/6.5.3/` (no sudo needed)

4. **Install**: Click "Next" and wait for installation (this may take 10-20 minutes)

### Step 3: Build Plugin with Qt 6.5.3

Once Qt 6.5.3 is installed:

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build

# Set Qt 6.5.3 path (adjust if you installed elsewhere)
export QT_PATH="$HOME/Qt/6.5.3/macos"
# OR if installed to /opt:
# export QT_PATH="/opt/qt6.5.3/6.5.3/macos"

# Verify Qt 6.5.3 is found
cmake -DCMAKE_PREFIX_PATH="$QT_PATH" \
      -DBUILD_DYNAMIC_PLUGIN=ON \
      -DENABLE_QT6=ON \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# Build MosaicPlanner plugin
make MosaicPlanner -j$(sysctl -n hw.ncpu)

# Install plugin
./../plugins/MosaicPlanner/install_plugin.sh
```

### Step 4: Verify Qt Version in Plugin

```bash
# Check Qt version embedded in plugin
strings ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep "Qt 6\."
```

**Expected Output**: `Qt 6.5.3`

### Step 5: Test with Production Stellarium

1. **Open Production Stellarium**:
   ```bash
   open /Applications/Stellarium.app
   ```

2. **Enable Plugin**:
   - Press `F2` → Plugins tab
   - Find **"Mosaic Planner"**
   - Click **"Load now"**

3. **Test**:
   ```javascript
   MosaicPlanner.flagMosaicMode = true;
   ```

**Result**: Plugin should load without Qt version errors!

---

## Alternative: Build Qt 6.5.3 from Source

If you need exact control or can't use the installer:

```bash
# Clone Qt repository
git clone https://code.qt.io/qt/qtbase.git
cd qtbase
git checkout v6.5.3

# Configure build
./configure -prefix /opt/qt6.5.3 \
            -release \
            -opensource \
            -confirm-license

# Build (this takes 2-4 hours on a modern Mac)
make -j$(sysctl -n hw.ncpu)

# Install
sudo make install
```

**Note**: This is time-consuming and not recommended unless you have specific requirements.

---

## Troubleshooting

### Plugin Doesn't Appear in List

1. **Check Plugin File**:
   ```bash
   ls -lh ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/
   ```

2. **Check Log**:
   ```bash
   tail -100 ~/Library/Application\ Support/Stellarium/log.txt | grep -i mosaic
   ```

3. **Common Issues**:
   - Qt version mismatch (plugin rejected)
   - Plugin file missing
   - Wrong Stellarium version (dev vs production)

### Plugin Loads But Mosaic Doesn't Display

1. **Check Mosaic Mode**:
   ```javascript
   MosaicPlanner.flagMosaicMode  // Should be true
   ```

2. **Check Equipment**:
   - Open Oculars plugin configuration
   - Ensure at least one CCD and Telescope are configured
   - MosaicPlanner uses the same equipment

3. **Check Settings**:
   ```javascript
   MosaicPlanner.mosaicPanelsX  // Should be > 0
   MosaicPlanner.mosaicPanelsY  // Should be > 0
   ```

### Qt Version Mismatch Errors

**Error**: `"The plugin uses incompatible Qt library. (X.X.X) [release]"`

**Solution**: Plugin must be built with Qt 6.5.3 to match production Stellarium.

**Fix**: Rebuild plugin with Qt 6.5.3 (see Option 2 above).

---

## Quick Reference

### Plugin Information
- **Name**: "Mosaic Planner"
- **ID**: `MosaicPlanner`
- **Settings**: `~/Library/Application Support/Stellarium/modules/MosaicPlanner/mosaicplanner.ini`
- **Equipment**: Uses Oculars plugin's `ocular.ini`

### Qt Versions
- **Production Stellarium**: Qt 6.5.3
- **Development Build**: Qt 5.15.17
- **Homebrew**: Qt 6.9.3 (too new)
- **Required for Production**: Qt 6.5.3

### Recommended Path
1. **For Testing**: Use development build (Option 1) - works immediately
2. **For Production**: Download Qt 6.5.3 (Option 2) - matches production

---

## Summary

**Current Status**:
- ✅ Plugin works with development build (Qt 5)
- ❌ Plugin doesn't work with production Stellarium (Qt 6.5.3 vs 6.9.3)

**Solutions**:
1. **Use Dev Build**: Build Stellarium with Qt 5, plugin works immediately
2. **Get Qt 6.5.3**: Download from Qt Company, rebuild plugin, works with production

**Next Steps**:
- Choose Option 1 if you just want to test the plugin
- Choose Option 2 if you need it to work with production Stellarium

