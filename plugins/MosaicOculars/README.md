# MosaicOculars Plugin

A fork of the Stellarium Oculars plugin that adds mosaic planning capabilities. This plugin provides all the functionality of the original Oculars plugin (eyepiece simulation, CCD frame overlay, Telrad sight) plus the ability to plan and visualize image mosaics with configurable X/Y panels and overlap percentage.

## Features

- **All Oculars Features**: Full compatibility with the original Oculars plugin
  - Eyepiece simulation
  - CCD/sensor frame overlay
  - Telrad sight
  - Equipment management (telescopes, eyepieces, CCDs, lenses)
  
- **Mosaic Planning**: 
  - Configurable X and Y panel counts (1-20 each)
  - Adjustable overlap percentage (0-50%)
  - Rotation angle support
  - Visual overlay showing all mosaic panels

## Building

This plugin is designed to be built as a **dynamic plugin**, independent of the Stellarium build process.

### Prerequisites

- Qt 6.5.3 or later (must match Stellarium's Qt version)
- CMake 3.10 or later
- C++ compiler with C++17 support

### Build Steps

1. **Navigate to the plugin directory**:
   ```bash
   cd plugins/MosaicOculars
   ```

2. **Run the build script**:
   ```bash
   ./build_plugin.sh
   ```

   The script will:
   - Detect your Qt installation
   - Configure CMake for dynamic plugin build
   - Build the plugin
   - Install it to your Stellarium user data directory
   - Fix library paths on macOS (if needed)

### Manual Build

If you prefer to build manually:

```bash
cd plugins/MosaicOculars
mkdir -p build && cd build

cmake -DBUILD_DYNAMIC_PLUGIN=ON \
      -DCMAKE_PREFIX_PATH=/path/to/qt \
      -DENABLE_QT6=ON \
      ../src

make -j$(nproc)
```

Then copy the built library to your Stellarium modules directory:
- **macOS**: `~/Library/Application Support/Stellarium/modules/MosaicOculars/`
- **Linux**: `~/.stellarium/modules/MosaicOculars/`
- **Windows**: `%APPDATA%\Stellarium\modules\MosaicOculars\`

## Installation

After building, the plugin should be automatically installed to your Stellarium user data directory. To use it:

1. **Restart Stellarium** (if it was running)
2. **Open Configuration** → **Plugins**
3. **Enable "Mosaic Oculars"** plugin
4. The plugin will appear in the main toolbar

## Usage

1. **Configure Equipment**: Use the settings dialog to add your telescopes, CCDs, and lenses
2. **Enable CCD Mode**: Click the sensor button to enable CCD frame overlay
3. **Enable Mosaic Mode**: Use the mosaic mode toggle (or keyboard shortcut)
4. **Adjust Mosaic Settings**:
   - Set X and Y panel counts
   - Adjust overlap percentage
   - Set rotation angle if needed
5. **Visualize**: The mosaic panels will be displayed as overlapping outlines on the sky

## Keyboard Shortcuts

- **Toggle Mosaic Mode**: Configure in Stellarium's shortcuts
- **Increase/Decrease X Panels**: Configure in shortcuts
- **Increase/Decrease Y Panels**: Configure in shortcuts
- **Reset Rotation**: Configure in shortcuts

## Settings

The plugin stores its settings in:
- **macOS**: `~/Library/Application Support/Stellarium/modules/MosaicOculars/mosaicocular.ini`
- **Linux**: `~/.stellarium/modules/MosaicOculars/mosaicocular.ini`
- **Windows**: `%APPDATA%\Stellarium\modules\MosaicOculars\mosaicocular.ini`

## License

GNU General Public License v2 or later (same as Stellarium)

## Credits

Based on the Stellarium Oculars plugin by Timothy Reaves, Bogdan Marinov, and Georg Zotti.

