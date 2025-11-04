# Qt Version Compatibility Issue

## Problem

The MosaicPlanner plugin cannot load in production Stellarium due to a Qt version mismatch:

- **Production Stellarium**: Uses Qt 6.5.3 (bundled in app)
- **Plugin Build**: Built with Qt 5.15.17 (Homebrew)
- **Error**: "The plugin uses incompatible Qt library. (5.15.0) [release]"

## Solutions

### Option 1: Build Plugin with Qt 6 (Recommended)

1. Install Qt 6:
   ```bash
   brew install qt6
   ```

2. Rebuild plugin with Qt 6:
   ```bash
   cd build
   cmake -DBUILD_DYNAMIC_PLUGIN=ON -DCMAKE_PREFIX_PATH=$(brew --prefix qt6) ..
   make MosaicPlanner
   ./../plugins/MosaicPlanner/install_plugin.sh
   ```

### Option 2: Use Development Build of Stellarium

Instead of production Stellarium, use the development build from this repository which uses Qt 5:

```bash
cd build
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..
make stellarium
./src/stellarium  # Runs with Qt 5, plugin will work
```

### Option 3: Copy Qt Frameworks (Complex)

Copy Qt 6 frameworks from production Stellarium into the plugin bundle. This is complex and not recommended.

## Current Status

- Plugin builds successfully with Qt 5
- Plugin loads correctly in development Stellarium (Qt 5)
- Plugin **cannot** load in production Stellarium (Qt 6)

## Recommendation

Use Option 2 (development build) for testing, or Option 1 if you need to test with production Stellarium.

