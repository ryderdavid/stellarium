# Installing Qt 6.5.3 Using Qt Online Installer

## Step 1: Download Installer

You've downloaded: `qt-online-installer-macOS-x64-4.10.0.dmg`

✅ **This is correct!** The version number (4.10.0) refers to the **installer version**, not the Qt version. This installer will let you select Qt 6.5.3.

## Step 2: Open and Run Installer

1. **Open the DMG file**:
   ```bash
   open ~/Downloads/qt-online-installer-macOS-x64-4.10.0.dmg
   ```
   Or double-click it in Finder.

2. **Run the installer**:
   - Drag the Qt installer app to your Applications folder (or run it directly)
   - Double-click to launch

## Step 3: Sign In / Create Account

1. **If you have a Qt account**:
   - Enter your email and password
   - Click "Sign In"

2. **If you don't have a Qt account**:
   - Click "Create Account" (it's free)
   - Fill in your details
   - Verify your email if required

## Step 4: Select Installation Path

1. **Choose installation directory**:
   - Default: `~/Qt/` (recommended, no sudo needed)
   - Alternative: `/opt/qt6.5.3/` (requires sudo)
   - Click "Next"

## Step 5: Select Qt 6.5.3

1. **In the component selection screen**:
   - Expand **Qt** section
   - Expand **Qt 6.5.3**
   - Check **macOS** (or **macOS (Apple Silicon)** if you have M1/M2 Mac)
   - Also check:
     - **Qt 6.5.3** → **Sources** (optional, for debugging)
     - **Developer and Designer Tools** → **Qt Creator** (optional, but recommended)

2. **Verify version**:
   - Make sure you see **Qt 6.5.3** in the list
   - Not Qt 6.6, 6.7, 6.8, or 6.9

3. **Click "Next"** to proceed

## Step 6: Accept License

1. **Read the license agreement**
2. **Check "I have read and agree to the terms"**
3. **Click "Next"**

## Step 7: Install

1. **Review installation summary**:
   - Should show Qt 6.5.3 will be installed
   - Installation path should be correct

2. **Click "Install"**
3. **Wait for installation** (this takes 10-20 minutes, depending on your internet speed)

## Step 8: Verify Installation

After installation completes:

```bash
# Check Qt 6.5.3 is installed
ls -la ~/Qt/6.5.3/macos/bin/qmake

# Check version
~/Qt/6.5.3/macos/bin/qmake -v
```

**Expected output**: Should show Qt version 6.5.3

## Step 9: Build Plugin with Qt 6.5.3

Once Qt 6.5.3 is installed:

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build

# Set Qt 6.5.3 path
export QT_PATH="$HOME/Qt/6.5.3/macos"

# Configure build
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

## Step 10: Verify Plugin Qt Version

```bash
# Check Qt version embedded in plugin
strings ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep "Qt 6\."
```

**Expected output**: `Qt 6.5.3`

## Troubleshooting

### Installer Won't Run

If the installer doesn't run:
```bash
# Make it executable
chmod +x /path/to/Qt\ Installer.app/Contents/MacOS/Qt\ Installer
```

### Can't Find Qt 6.5.3 in List

If Qt 6.5.3 doesn't appear:
1. Check your internet connection
2. The installer downloads available versions from Qt servers
3. Qt 6.5.3 should be available (it's a stable release)
4. Try updating the installer first

### Installation Fails

If installation fails:
1. Check you have enough disk space (Qt 6.5.3 needs ~2-3 GB)
2. Check internet connection
3. Try installing to a different location
4. Check Qt installer logs

### Wrong Architecture

If you have an Apple Silicon Mac (M1/M2/M3):
- Select **macOS (Apple Silicon)** in the installer
- Path will be: `~/Qt/6.5.3/macos` (same path, but ARM64 binaries)

If you have an Intel Mac:
- Select **macOS** (x86_64)
- Path will be: `~/Qt/6.5.3/macos` (x86_64 binaries)

## Summary

✅ **Download**: `qt-online-installer-macOS-x64-4.10.0.dmg` - Correct!  
✅ **Install**: Run installer, sign in, select Qt 6.5.3  
✅ **Build**: Use `~/Qt/6.5.3/macos` as CMAKE_PREFIX_PATH  
✅ **Verify**: Plugin should show Qt 6.5.3 version

## Next Steps

After installing Qt 6.5.3:
1. Build plugin with Qt 6.5.3 (see Step 9 above)
2. Test plugin with production Stellarium
3. Plugin should load without Qt version errors!

