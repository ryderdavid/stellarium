# Building MosaicPlanner with Qt 6

## Status

✅ **Plugin successfully rebuilt with Qt 6.9.3**

The plugin has been rebuilt to use Qt 6, which should be compatible with production Stellarium (Qt 6.5.3).

## Build Configuration

```bash
cd build
cmake -DBUILD_DYNAMIC_PLUGIN=ON \
      -DCMAKE_PREFIX_PATH=$(brew --prefix qt) \
      -DENABLE_QT6=ON ..
make MosaicPlanner
```

## Installation

```bash
./plugins/MosaicPlanner/install_plugin.sh
```

## Qt Version Compatibility

- **Plugin**: Built with Qt 6.9.3 (Homebrew)
- **Production Stellarium**: Uses Qt 6.5.3 (bundled)
- **Compatibility**: Qt 6.x maintains ABI compatibility, so the plugin should work

## Verification

Check Qt libraries linked:
```bash
otool -L ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/libMosaicPlanner.dylib | grep Qt
```

Should show:
- QtWidgets.framework (6.0.0 compatibility, 6.9.3 current)
- QtGui.framework (6.0.0 compatibility, 6.9.3 current)  
- QtCore.framework (6.0.0 compatibility, 6.9.3 current)

## Testing

1. Restart Stellarium
2. Check Plugins list - MosaicPlanner should appear
3. Enable the plugin
4. Test mosaic mode via scripting:
   ```javascript
   MosaicPlanner.flagMosaicMode = true;
   MosaicPlanner.mosaicPanelsX = 3;
   MosaicPlanner.mosaicPanelsY = 3;
   ```

## If Issues Persist

If the plugin still doesn't load, check Stellarium log:
```bash
tail -50 ~/Library/Application\ Support/Stellarium/log.txt | grep -i mosaic
```

Common issues:
- Qt version mismatch: Plugin must match Stellarium's Qt major version
- Missing dependencies: Check for missing Qt modules
- Plugin not discovered: Verify directory structure and file permissions

