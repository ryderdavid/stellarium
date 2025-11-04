# Matching Qt Version for Production Stellarium

## Current Situation

- **Production Stellarium**: Uses **Qt 6.5.3** (bundled in app)
- **Plugin Build**: Built with **Qt 6.9.3** (Homebrew)
- **Error**: Qt plugin loader rejects version mismatch: `"The plugin uses incompatible Qt library. (6.9.0) [release]"`

## Finding Qt 6.5.3

### Option 1: Download Qt 6.5.3 from Qt Company

Qt provides official installers for specific versions:

1. **Visit Qt Download Page**:
   - https://www.qt.io/download-qt-installer
   - Or direct: https://www.qt.io/download-qt-installer-oss

2. **Download Qt 6.5.3**:
   - Select Qt 6.5.3 from version dropdown
   - Choose macOS installer
   - Install to a custom location (e.g., `/opt/qt6.5.3`)

3. **Build Plugin with Qt 6.5.3**:
   ```bash
   cd build
   cmake -DBUILD_DYNAMIC_PLUGIN=ON \
         -DCMAKE_PREFIX_PATH=/opt/qt6.5.3/6.5.3/macos \
         -DENABLE_QT6=ON ..
   make MosaicPlanner
   ```

### Option 2: Use Qt Maintenance Tool

If you have Qt installed via Qt installer:

1. **Open Qt Maintenance Tool**:
   ```bash
   /path/to/Qt/MaintenanceTool.app
   ```

2. **Install Qt 6.5.3**:
   - Go to "Add or Remove Components"
   - Select Qt 6.5.3
   - Install to desired location

3. **Build Plugin**:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.3/macos ..
   ```

### Option 3: Build Qt 6.5.3 from Source

If you need exact version match:

1. **Clone Qt Repository**:
   ```bash
   git clone https://code.qt.io/qt/qt5.git  # Actually qtbase for Qt6
   cd qtbase
   git checkout v6.5.3
   ```

2. **Build Qt 6.5.3**:
   ```bash
   ./configure -prefix /opt/qt6.5.3
   make -j$(sysctl -n hw.ncpu)
   make install
   ```

**Note**: This is time-consuming (several hours)

### Option 4: Use Homebrew Formula for Specific Version

Check if there's a Homebrew formula for Qt 6.5:

```bash
brew search qt@6.5
brew install qt@6.5  # If available
```

## Current Status

**Production Stellarium Version**: 25.2  
**Qt Version Required**: 6.5.3  
**Qt Version Available**: 6.9.3 (Homebrew)  
**Compatibility**: Qt 6.x should be compatible, but plugin loader is strict about version

## Recommended Approach

### For Testing: Use Development Build

Use the development build of Stellarium (see `DEV_BUILD_INSTRUCTIONS.md`):
- Uses Qt 5 (matches plugin build)
- Works immediately
- Easy to test and debug

### For Production: Match Qt Version

To work with production Stellarium:
1. Download Qt 6.5.3 from Qt Company
2. Build plugin with Qt 6.5.3
3. Install to production Stellarium

## Verification

After building with Qt 6.5.3, verify the plugin:

```bash
# Check Qt version in plugin
strings ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep "Qt 6\."
```

Should show: `Qt 6.5.3`

## Alternative: Use Qt Online Installer

Qt provides an online installer that allows selecting specific versions:

1. Download: https://www.qt.io/download-qt-installer
2. Install Qt 6.5.3
3. Set CMAKE_PREFIX_PATH to installed location
4. Rebuild plugin

