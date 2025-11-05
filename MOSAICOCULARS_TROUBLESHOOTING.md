# MosaicOculars Plugin Troubleshooting Guide

## Project Overview

We are developing a **MosaicOculars** plugin for Stellarium, which is a fork of the existing **Oculars** plugin with additional mosaic planning capabilities. The plugin should:

1. **Function as a standalone dynamic plugin** - Built independently of Stellarium core, installable to production Stellarium
2. **Provide all Oculars functionality** - CCD/Telescope/Ocular/Lens management, FOV overlays, etc.
3. **Add mosaic planning features** - Ability to visualize X×Y grids of overlapping panels with configurable rotation and overlap percentage

## Current Status

### ✅ What's Working

1. **Plugin Build & Installation**
   - Plugin compiles successfully as a dynamic library (`.dylib` on macOS)
   - Qt6 (version 6.5.3) compatibility is configured
   - Plugin appears in Stellarium's plugin list
   - Plugin can be enabled/disabled
   - Plugin loads without crashing Stellarium

2. **Drawing/Rendering**
   - Plugin's `draw()` method is being called (confirmed by visible debug box in top-right corner)
   - Debug box renders correctly (red filled rectangle with white outline)

3. **Configuration Dialog**
   - Dialog class (`MosaicOcularsDialog`) exists and is properly structured
   - Property connections updated from "Oculars.*" to "MosaicOculars.*"
   - Dialog should manage equipment (CCDs, Telescopes, Oculars, Lenses)

### ❌ What's NOT Working

1. **GUI Panel Not Appearing**
   - The `MosaicOcularsGuiPanel` (the on-screen control panel) is not visible
   - Panel should show buttons for Ocular/CCD/Telrad modes, configuration button, and control fields
   - Panel creation is attempted but fails silently

2. **Configuration Button Greyed Out**
   - The "Configure" button in Stellarium's plugin configuration window is disabled
   - This suggests the `configureGui()` method or plugin readiness state is not being recognized

3. **StelGui Availability Issue**
   - Console logs show: `MosaicOculars::draw() - StelGui not available` repeatedly
   - `StelGui` and `SkyGui` are not available when plugin attempts to create GUI panel
   - Retry timer mechanism (checking every 100ms) was implemented but doesn't resolve the issue

## Technical Implementation Details

### Plugin Structure

```
plugins/MosaicOculars/
├── src/
│   ├── MosaicOculars.hpp/cpp        # Main plugin class
│   ├── MosaicOcularsDialog.hpp/cpp  # Configuration dialog
│   ├── MosaicOcularsGuiPanel.hpp/cpp # On-screen control panel
│   ├── CCD.hpp/cpp                  # Equipment classes
│   ├── Telescope.hpp/cpp
│   ├── Ocular.hpp/cpp
│   └── Lens.hpp/cpp
├── resources/
│   └── MosaicOculars.qrc            # Qt resource file
└── CMakeLists.txt
```

### Key Classes

1. **MosaicOculars** (extends `StelModule`)
   - Main plugin class
   - Implements `StelPluginInterface` for dynamic loading
   - Handles equipment management, FOV calculations, drawing
   - Location: `plugins/MosaicOculars/src/MosaicOculars.{hpp,cpp}`

2. **MosaicOcularsDialog** (extends `StelDialog`)
   - Configuration window for managing equipment
   - Accessed via plugin configuration button
   - Location: `plugins/MosaicOculars/src/gui/MosaicOcularsDialog.{hpp,cpp}`

3. **MosaicOcularsGuiPanel** (extends `QGraphicsWidget`)
   - On-screen control panel with buttons and info fields
   - Should appear in sky view when enabled
   - Location: `plugins/MosaicOculars/src/gui/MosaicOcularsGuiPanel.{hpp,cpp}`

### Critical Code Sections

#### 1. Plugin Initialization (`MosaicOculars::init()`)

```cpp
void MosaicOculars::init()
{
    // Load settings from mosaicocular.ini
    validateAndLoadIniFile();
    
    // Create configuration dialog
    ocularDialog = new MosaicOcularsDialog(this, &ccds, &oculars, &telescopes, &lenses);
    
    // Initialize actions (keyboard shortcuts, etc.)
    initializeActivationActions();
    
    // Attempt to create GUI panel
    if (flagGuiPanelEnabled)
    {
        enableGuiPanel(true);
        
        // Retry timer to wait for StelGui to become available
        QTimer* guiRetryTimer = new QTimer(this);
        guiRetryTimer->setInterval(100);
        // ... retry logic ...
    }
}
```

#### 2. GUI Panel Creation (`MosaicOculars::enableGuiPanel()`)

```cpp
void MosaicOculars::enableGuiPanel(bool enable)
{
    if (enable)
    {
        if (!guiPanel)
        {
            StelGui* gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
            if (gui)
            {
                QGraphicsWidget* skyGui = gui->getSkyGui();
                if (skyGui)
                {
                    guiPanel = new MosaicOcularsGuiPanel(this, skyGui);
                    // ... show appropriate mode ...
                }
            }
        }
    }
    // ...
}
```

#### 3. Configuration Dialog (`MosaicOculars::configureGui()`)

```cpp
bool MosaicOculars::configureGui(bool show)
{
    if (show)
    {
        ocularDialog->setVisible(true);
    }
    return ready;
}
```

### Debugging Attempts

1. **Added Debug Drawing**
   - Red box in top-right corner confirms `draw()` is called
   - Shows plugin is active and rendering

2. **Added Logging**
   - Console output shows `StelGui` not available during `draw()`
   - Retry timer logs show continuous retries without success

3. **GUI Panel Retry Mechanism**
   - Implemented `QTimer` that checks every 100ms for `StelGui` availability
   - Timer stops after 10 seconds (100 retries) if unsuccessful
   - Issue: `StelGui` never becomes available, or timing is wrong

4. **Property Connection Fixes**
   - Updated all "Oculars.*" property references to "MosaicOculars.*"
   - Ensures configuration dialog connects to correct plugin instance

### Build Configuration

**CMake Setup:**
- Plugin builds as `MODULE` library (dynamic)
- Uses Qt6 (version 6.5.3) to match Stellarium's Qt version
- macOS-specific: Uses `-undefined dynamic_lookup` for symbol resolution
- Qt library paths fixed with `install_name_tool` to point to Stellarium's bundled Qt

**Build Command:**
```bash
cd plugins/MosaicOculars
./build_plugin.sh
```

## The Problem: Root Cause Analysis

### Hypothesis 1: Timing Issue
**Problem:** `StelGui` is not initialized when plugin's `init()` is called.

**Evidence:**
- Console shows `StelGui not available` repeatedly
- Retry timer runs but never succeeds
- Original Oculars plugin (static) might initialize at different time

**Possible Solutions:**
- Wait for a Stellarium signal that GUI is ready
- Use a different initialization point
- Check if static plugins get different treatment

### Hypothesis 2: Plugin Registration Issue
**Problem:** Plugin's `configureGui()` method isn't being recognized by Stellarium.

**Evidence:**
- Configuration button is greyed out in plugin list
- Suggests Stellarium doesn't see `configureGui()` override

**Possible Solutions:**
- Verify `StelModule::configureGui()` is properly overridden
- Check plugin interface registration
- Ensure plugin is fully registered before configuration is accessed

### Hypothesis 3: Dynamic Plugin Limitations
**Problem:** Dynamic plugins have different lifecycle/initialization than static plugins.

**Evidence:**
- Original Oculars works as static plugin
- Our fork as dynamic plugin has GUI issues
- Different timing/initialization order

**Possible Solutions:**
- Compare initialization order with static Oculars
- Check if dynamic plugins need different approach
- Review Stellarium's dynamic plugin documentation

## What Success Looks Like

### Success Criteria

1. **GUI Panel Visible**
   - On-screen control panel appears when plugin is enabled
   - Shows buttons: Ocular, CCD, Telrad, Configuration
   - Shows information fields for selected equipment
   - Panel position and layout match original Oculars behavior

2. **Configuration Dialog Accessible**
   - "Configure" button in plugin list is enabled (not greyed out)
   - Clicking "Configure" opens `MosaicOcularsDialog`
   - Can add/edit/delete equipment (CCDs, Telescopes, Oculars, Lenses)
   - Settings save and load correctly

3. **Equipment Management**
   - Can select CCD/Telescope/Lens combinations
   - FOV overlays render correctly
   - Equipment data persists in `mosaicocular.ini`

4. **Mosaic Planning (Future)**
   - Can enable mosaic mode
   - Can set X/Y panel counts
   - Can set rotation angle and overlap percentage
   - Mosaic grid renders on sky view

## Files to Review

### Critical Files
1. `plugins/MosaicOculars/src/MosaicOculars.cpp`
   - Lines 578-830: `init()` method
   - Lines 243-251: `configureGui()` method
   - Lines 917-948: `enableGuiPanel()` method
   - Lines 1056-1070: `draw()` method (with debug box)

2. `plugins/MosaicOculars/src/MosaicOculars.hpp`
   - Plugin class declaration
   - Method signatures

3. `plugins/MosaicOculars/src/gui/MosaicOcularsGuiPanel.cpp`
   - Constructor and initialization
   - Panel creation logic

4. `plugins/MosaicOculars/src/gui/MosaicOcularsDialog.cpp`
   - Lines 287-370: `createDialogContent()` method
   - Property connections

### Comparison Files (Original Oculars)
- `plugins/Oculars/src/Oculars.cpp` - Compare initialization
- `plugins/Oculars/src/gui/OcularsGuiPanel.cpp` - Compare panel creation

## Specific Questions to Investigate

1. **When does StelGui become available?**
   - Is there a signal we should listen to?
   - What's the initialization order for static vs dynamic plugins?

2. **How does Stellarium detect `configureGui()`?**
   - Is it based on method signature?
   - Does it check plugin interface?
   - Is there a registration step we're missing?

3. **Are there differences in plugin lifecycle?**
   - Do dynamic plugins get different initialization timing?
   - Do they need explicit registration somewhere?

4. **Plugin Interface Registration**
   - Is `StelPluginInterface` properly implemented?
   - Is `Q_PLUGIN_METADATA` correctly configured?
   - Are there any missing interface methods?

## Console Output

When plugin is enabled, console shows:
```
[   10.545][DBG ] MosaicOculars::draw() - StelGui not available
[   10.612][DBG ] MosaicOculars::draw() - StelGui not available
[   10.675][DBG ] MosaicOculars::draw() - StelGui not available
...
```

This repeats continuously, indicating `StelGui` is never available when checked.

## Next Steps for Troubleshooting

1. **Compare with static Oculars initialization**
   - Check when static Oculars creates its GUI panel
   - Look for signals or events it listens to
   - Compare initialization order

2. **Investigate StelGui lifecycle**
   - Find when StelGui becomes available
   - Check for signals that indicate GUI readiness
   - Look for examples in other plugins

3. **Check plugin interface registration**
   - Verify `Q_PLUGIN_METADATA` is correct
   - Check if `getPluginInfo()` returns correct data
   - Ensure plugin is fully registered

4. **Test configuration dialog independently**
   - Try calling `ocularDialog->setVisible(true)` directly
   - Check if dialog can be created without GUI panel
   - Verify dialog doesn't depend on GUI panel

5. **Review Stellarium plugin documentation**
   - Check for dynamic plugin-specific requirements
   - Look for initialization best practices
   - Find examples of working dynamic plugins

## Environment

- **OS:** macOS (darwin 25.0.0)
- **Stellarium:** Production version (Qt 6.5.3)
- **Qt Version:** 6.5.3 (installed via Qt Online Installer)
- **Build System:** CMake with Qt6
- **Plugin Type:** Dynamic plugin (`.dylib`)

## Plugin Installation Location

```
~/Library/Application Support/Stellarium/modules/MosaicOculars/libMosaicOculars.dylib
```

## Quick Reference: Key File Locations

### Plugin Source Files
- **Main Plugin:** `plugins/MosaicOculars/src/MosaicOculars.{hpp,cpp}`
- **Configuration Dialog:** `plugins/MosaicOculars/src/gui/MosaicOcularsDialog.{hpp,cpp}`
- **GUI Panel:** `plugins/MosaicOculars/src/gui/MosaicOcularsGuiPanel.{hpp,cpp}`
- **Equipment Classes:** `plugins/MosaicOculars/src/{CCD,Telescope,Ocular,Lens}.{hpp,cpp}`
- **Build Script:** `plugins/MosaicOculars/build_plugin.sh`

### Settings File
- **User Data:** `~/Library/Application Support/Stellarium/modules/MosaicOculars/mosaicocular.ini`
- **Plugin Library:** `~/Library/Application Support/Stellarium/modules/MosaicOculars/libMosaicOculars.dylib`

### Comparison Files (Original Oculars)
- `plugins/Oculars/src/Oculars.{hpp,cpp}`
- `plugins/Oculars/src/gui/OcularsGuiPanel.{hpp,cpp}`
- `plugins/Oculars/src/gui/OcularDialog.{hpp,cpp}`

## Summary

The MosaicOculars plugin builds and loads successfully, but the GUI components (both the on-screen panel and the configuration dialog button) are not accessible. The core issue appears to be related to the timing of GUI initialization in dynamic plugins versus static plugins, or potentially a missing step in plugin registration that prevents Stellarium from recognizing the plugin's configuration capabilities.

The debug box confirms the plugin is active and rendering, so the issue is specifically with GUI component initialization and accessibility, not with the plugin's core functionality or drawing capabilities.

## Recommended Investigation Order

1. **First:** Compare static Oculars initialization with MosaicOculars
   - Check when `enableGuiPanel()` is called in original
   - Look for any signals or events related to GUI readiness
   - Compare the exact initialization sequence

2. **Second:** Investigate `configureGui()` recognition
   - Check if method signature matches exactly
   - Verify virtual method override is correct
   - Check if plugin interface registration is complete

3. **Third:** Test configuration dialog independently
   - Try creating/showing dialog directly
   - Check if it's a dependency issue vs initialization issue
   - Verify dialog can be created without GUI panel

4. **Fourth:** Review Stellarium plugin architecture
   - Check documentation for dynamic plugin requirements
   - Look for lifecycle differences between static/dynamic
   - Find examples of working dynamic plugins with GUI

