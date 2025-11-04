# Mosaic Planner Plugin

A standalone Stellarium plugin for planning and visualizing multi-panel imaging mosaics with configurable overlap and rotation.

## Features

- **Independent Plugin**: Runs separately from the Oculars plugin, avoiding conflicts
- **Equipment Integration**: Loads CCD, Telescope, and Lens data from Oculars configuration
- **Mosaic Visualization**: Displays N×M panel grid with configurable overlap
- **Rotation Support**: Rotate the entire mosaic grid by any angle
- **Dynamic Plugin**: Can be installed to production Stellarium without rebuilding

## Building

### As Dynamic Plugin (Recommended for Testing)

```bash
cd build
cmake -DBUILD_DYNAMIC_PLUGIN=ON -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@5 ..
make MosaicPlanner
```

### Installation

Run the installation script from the repository root:

```bash
./plugins/MosaicPlanner/install_plugin.sh
```

Or manually:
```bash
# macOS
mkdir -p ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner
cp build/plugins/MosaicPlanner/src/libMosaicPlanner.dylib ~/Library/Application\ Support/Stellarium/modules/MosaicPlanner/

# Linux
mkdir -p ~/.stellarium/modules/MosaicPlanner
cp build/plugins/MosaicPlanner/src/libMosaicPlanner.so ~/.stellarium/modules/MosaicPlanner/

# Windows
mkdir %APPDATA%\Stellarium\modules\MosaicPlanner
copy build\plugins\MosaicPlanner\src\MosaicPlanner.dll %APPDATA%\Stellarium\modules\MosaicPlanner\
```

## Usage

1. **Restart Stellarium** after installation
2. **Enable the Plugin**: Go to Configuration → Plugins → MosaicPlanner → Enable
3. **Configure Equipment**: The plugin loads equipment from `modules/Oculars/ocular.ini` (same as Oculars plugin)
4. **Enable Mosaic Mode**: Use the property system or scripting to set `flagMosaicMode` to `true`
5. **Configure Mosaic**:
   - `mosaicPanelsX`: Number of panels in X direction (1-20, default: 3)
   - `mosaicPanelsY`: Number of panels in Y direction (1-20, default: 3)
   - `mosaicRotationAngle`: Rotation angle in degrees (0-360, default: 0)
   - `mosaicOverlapPercent`: Overlap percentage (0-50, default: 20)

## Scripting Example

```javascript
// Enable mosaic mode
MosaicPlanner.flagMosaicMode = true;

// Configure a 5×3 mosaic
MosaicPlanner.mosaicPanelsX = 5;
MosaicPlanner.mosaicPanelsY = 3;

// Set 25% overlap
MosaicPlanner.mosaicOverlapPercent = 25.0;

// Rotate by 45 degrees
MosaicPlanner.mosaicRotationAngle = 45.0;
```

## Properties

All configuration is exposed as Qt properties:

- `flagMosaicMode` (bool): Enable/disable mosaic visualization
- `mosaicPanelsX` (int): X-axis panel count (1-20)
- `mosaicPanelsY` (int): Y-axis panel count (1-20)
- `mosaicRotationAngle` (double): Grid rotation in degrees (0-360)
- `mosaicOverlapPercent` (double): Overlap percentage (0-50)

## Equipment Data

The plugin reads equipment (CCD, Telescope, Lens) from the same `ocular.ini` file used by the Oculars plugin:

- Location: `modules/Oculars/ocular.ini` (in Stellarium user data directory)
- Format: Same as Oculars plugin configuration
- Equipment is automatically loaded on plugin initialization

If you have equipment configured in Oculars, MosaicPlanner will use the same equipment automatically.

## Technical Details

- **Plugin ID**: `MosaicPlanner`
- **Dependencies**: None (standalone plugin)
- **Equipment Classes**: Copied from Oculars (CCD, Telescope, Lens)
- **Rendering**: Uses StelPainter with LineLoop mode for panel outlines

## Troubleshooting

### Plugin Not Appearing

1. Check that the plugin file exists in `modules/MosaicPlanner/`
2. Check Stellarium log for loading errors
3. Ensure you're using a compatible Stellarium version

### Equipment Not Loading

1. Verify `modules/Oculars/ocular.ini` exists
2. Ensure equipment is configured in Oculars first
3. Check plugin log output for equipment loading messages

### Mosaic Not Displaying

1. Ensure `flagMosaicMode` is set to `true`
2. Verify equipment is selected (CCD and Telescope indices)
3. Check that panels are calculated (look for log messages)

## License

GNU General Public License v2 or later

