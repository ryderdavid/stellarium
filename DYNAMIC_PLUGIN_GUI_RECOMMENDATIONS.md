# Dynamic Plugin GUI Recommendations

**Note:** This document was created during development when mosaic functionality was being developed as a dynamic plugin. The functionality has since been integrated into the static Oculars plugin, so this document is kept for reference only.

## Problem Summary

**Issue:** `StelApp::getInstance().getGui()` returns NULL for dynamic plugins in production Stellarium, preventing access to `StelGui` and `SkyGui` needed for creating on-screen GUI panels like `OcularsGuiPanel`.

**Evidence:**
- Log shows: `StelGui is NULL` continuously during `update()` calls
- The plugin loads successfully and `update()` is called
- Configure button works (dialog-based GUI is functional)
- Only the on-screen overlay GUI panel (like Oculars has) doesn't work

## Working Features

✅ Plugin loads and initializes successfully
✅ Configure button is enabled in plugin list
✅ Configuration dialog (`MosaicOcularsDialog`) works
✅ `update()` method is called every frame
✅ Keyboard actions are registered
✅ Equipment management (CCD, Telescope, Ocular, Lens) works

## Not Working

❌ On-screen GUI panel (`MosaicOcularsGuiPanel`) - requires `StelGui::getSkyGui()`
❌ Live control buttons for Ocular/CCD/Telrad modes on screen

## Root Cause

Production Stellarium binary doesn't expose `StelGui` to dynamic plugins via `StelApp::getInstance().getGui()`. This appears to be either:
1. A limitation of how production Stellarium is compiled
2. An architectural difference between static and dynamic plugins
3. A timing issue that never resolves (StelGui stays NULL indefinitely)

## Alternative Approaches

### Option 1: Use Bottom Toolbar Buttons (Recommended)
- Add buttons to `BottomStelBar` using `StelButton`
- This is documented as working for dynamic plugins
- Provides quick access without needing SkyGui
- **Action:** Implement toolbar buttons for mode switching

### Option 2: Keyboard Shortcuts Only
- Already implemented and working
- Users control everything via keyboard
- Document the shortcuts clearly
- **Pros:** Works now, no changes needed
- **Cons:** Less discoverable, not as user-friendly

### Option 3: Dialog-Based Control Panel
- Move all controls into the configuration dialog
- Add "Show Controls" button that opens a non-modal control dialog
- **Pros:** Works with current dynamic plugin limitations
- **Cons:** Less convenient than on-screen overlay

### Option 4: Make it a Static Plugin
- Integrate MosaicOculars into Stellarium source build
- Requires users to build Stellarium from source
- **Pros:** Full access to StelGui, exactly like Oculars
- **Cons:** Not distributable as standalone plugin

### Option 5: Hybrid Approach
- Use configuration dialog for equipment setup
- Use bottom toolbar for mode switching
- Use keyboard shortcuts for advanced controls
- **Best balance of usability and compatibility**

## Recommended Path Forward

**Short term:**
1. Keep configuration dialog working (already done ✅)
2. Add bottom toolbar buttons for mode switching
3. Document keyboard shortcuts in dialog/readme

**Long term consideration:**
- If bottom toolbar doesn't work either, accept keyboard-only control
- Document this as a known limitation of dynamic plugins
- Consider submitting to Stellarium as static plugin if adoption grows

## Technical Notes

### What We Tried
1. ✅ Implementing `update()` method - works, gets called every frame
2. ✅ Continuous checking for StelGui - works but StelGui stays NULL
3. ✅ Using QTimer with delays - doesn't help, StelGui never appears
4. ❌ Direct StelGui access via `StelApp::getInstance().getGui()` - always returns NULL

### Key Findings

1. **StelGui Access**: The issue is NOT timing - even after 100+ frames (several seconds), `StelApp::getInstance().getGui()` continues to return NULL for dynamic plugins. This suggests a fundamental architectural limitation.

2. **No Other Plugins Use On-Screen Panels**: Analysis of all built-in Stellarium plugins reveals that **NONE of them use on-screen GUI panels like OcularsGuiPanel**. All plugins use one of these approaches:
   - `StelDialog` subclasses (configuration windows opened via Configure button)
   - Direct rendering via `draw()` method
   - Keyboard shortcuts via `StelAction`

3. **OcularsGuiPanel is Unique**: The on-screen control panel style used by Oculars appears to be:
   - Unique to the Oculars plugin
   - Only works because Oculars is a **static plugin** compiled into Stellarium
   - Not a standard pattern for dynamic plugins

## Conclusion

**The OcularsGuiPanel-style on-screen GUI is NOT compatible with dynamic plugins**. This is not a bug in our implementation - it's a fundamental architectural difference between static and dynamic plugins in Stellarium.

All standard dynamic plugins use `StelDialog` for their GUI, which MosaicOculars already has working (Configure button is enabled and functional).

## Files Modified

- `plugins/MosaicOculars/src/MosaicOculars.hpp` - Added `update()` override
- `plugins/MosaicOculars/src/MosaicOculars.cpp` - Implemented continuous StelGui checking in `update()`

## Next Steps

1. Review full Stellarium developer documentation for dynamic plugin GUI examples
2. Test bottom toolbar button implementation
3. If toolbar works: implement mode switching buttons
4. If toolbar doesn't work: document keyboard shortcuts and accept limitation
5. Update user documentation with working control methods
