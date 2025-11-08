# Bug Fix Attempts - Oculars Plugin Issues

## Date: November 7, 2025

## Issues Reported

### Issue 1: Overlap Control Text Styling
**Problem**: The text in the GUI panel for the "Overlap" control appears in black and is a slightly different size than the rest of the text in that panel (X mosaic panels, Y mosaic panels, rotation, gear selection, etc.).

**Location**: `plugins/Oculars/src/gui/OcularsGuiPanel.cpp` - The `fieldMosaicOverlap` QGraphicsTextItem

### Issue 2: DSO Catalog Version Mismatch
**Problem**: Error message: "DSO catalog version mismatch. Found: 3.21 - Expected: 3.20. See Logfile for instructions."

**Location**: `src/core/modules/NebulaMgr.cpp` - The `StellariumDSOCatalogVersion` constant

---

## Attempted Fixes

### Fix Attempt 1: Added fieldMosaicOverlap to setControlsColor() and setControlsFont()

**What was done:**
- Added `fieldMosaicOverlap` to the `setControlsColor()` method (line ~1273)
- Added `fieldMosaicOverlap` to the `setControlsFont()` method (line ~1340)

**Code changes:**
```cpp
// In setControlsColor():
if (fieldMosaicOverlap)
    fieldMosaicOverlap->setDefaultTextColor(color);

// In setControlsFont():
if (fieldMosaicOverlap)
    fieldMosaicOverlap->setFont(font);
```

**Why it didn't work:**
- The `setControlsFont()` is called in the constructor (line 143) BEFORE the mosaic controls are created (lines 159-162)
- The `setControlsColor()` is called via `setColorScheme()` (line 236) AFTER the constructor, but the font was never set initially
- When `updateMosaicControls()` is called later, it uses `setPlainText()` which may reset the styling

### Fix Attempt 2: Set font explicitly after creating mosaic controls

**What was done:**
- Added code in the constructor (lines 164-174) to set the font for all mosaic controls immediately after creation:

```cpp
// Set font for mosaic controls (they were created after setControlsFont was called)
QFont mosaicFont = font();
mosaicFont.setPixelSize(plugin->getGuiPanelFontSize());
if (fieldMosaicMode)
    fieldMosaicMode->setFont(mosaicFont);
if (fieldMosaicPanelsX)
    fieldMosaicPanelsX->setFont(mosaicFont);
if (fieldMosaicPanelsY)
    fieldMosaicPanelsY->setFont(mosaicFont);
if (fieldMosaicOverlap)
    fieldMosaicOverlap->setFont(mosaicFont);
```

**Why it didn't work:**
- The font is set in the constructor, but `updateMosaicControls()` is called later and uses `setPlainText()` which may reset the font
- The color is still not being set when the text is updated in `updateMosaicControls()`

### Fix Attempt 3: Set color explicitly in updateMosaicControls()

**What was done:**
- Added code in `updateMosaicControls()` (line ~1150) to set the color after setting the text:

```cpp
// Overlap control
QString overlapLabel = QString(q_("Overlap: %1%")).arg(ocularsPlugin->getMosaicOverlapPercent(), 0, 'f', 0);
fieldMosaicOverlap->setPlainText(overlapLabel);
// Ensure color matches other controls (get color from another field)
fieldMosaicOverlap->setDefaultTextColor(fieldCcdRotation->defaultTextColor());
fieldMosaicOverlap->setPos(posX, posY);
fieldMosaicOverlap->setVisible(true);
```

**Why it didn't work:**
- This should work, but the user reports it still doesn't. Possible reasons:
  - The font size is still not matching (only color was addressed)
  - The color might not be getting applied correctly
  - There might be a timing issue where the color is set but then overridden

### Fix Attempt 4: Updated DSO Catalog Version

**What was done:**
- Changed `StellariumDSOCatalogVersion` from `"3.20"` to `"3.21"` in `src/core/modules/NebulaMgr.cpp` (line 55)

**Code change:**
```cpp
const QString NebulaMgr::StellariumDSOCatalogVersion = QStringLiteral("3.21");
```

**Why it didn't work:**
- The code change was made correctly
- The catalog file in the build directory contains version 3.21
- However, the user still sees the error. Possible reasons:
  - The app bundle might have an older catalog file cached
  - The catalog file path resolution might be finding a different file
  - The version check logic might have an issue

---

## Current Code State

### OcularsGuiPanel.cpp

**Constructor (lines 158-174):**
- Mosaic controls are created after `setControlsFont()` is called
- Font is set explicitly for mosaic controls after creation
- `setColorScheme()` is called at the end of the constructor (line 236)

**setControlsColor() (lines 1211-1275):**
- Includes `fieldMosaicOverlap` with conditional check
- Sets `setDefaultTextColor()` for all mosaic controls

**setControlsFont() (lines 1277-1341):**
- Includes `fieldMosaicOverlap` with conditional check
- Sets `setFont()` for all mosaic controls

**updateMosaicControls() (lines 1068-1163):**
- Sets text for all mosaic controls using `setPlainText()`
- For `fieldMosaicOverlap` (lines 1146-1152), it:
  - Sets the text
  - Sets the color by copying from `fieldCcdRotation->defaultTextColor()`
  - Sets position and visibility

**Note**: The other mosaic controls (`fieldMosaicMode`, `fieldMosaicPanelsX`, `fieldMosaicPanelsY`) do NOT have their color explicitly set in `updateMosaicControls()`, yet they apparently work correctly. This suggests the issue might be specific to `fieldMosaicOverlap`.

### NebulaMgr.cpp

**Line 55:**
```cpp
const QString NebulaMgr::StellariumDSOCatalogVersion = QStringLiteral("3.21");
```

**Catalog loading (lines 1543-1598):**
- Reads version from catalog file
- Compares with `StellariumDSOCatalogVersion`
- Shows warning if mismatch

---

## Analysis of Why Fixes Failed

### Issue 1: Overlap Text Styling

**Root Cause Hypothesis:**
1. **Font Size**: The font might be getting reset when `setPlainText()` is called, or the font size calculation might be different
2. **Color**: The color might not be persisting, or `setDefaultTextColor()` might not be the right method to use
3. **Timing**: The color/font might be set but then immediately overridden by something else
4. **Inconsistency**: Other mosaic controls work, so there might be something specific about how `fieldMosaicOverlap` is handled

**Key Observations:**
- Other mosaic controls (`fieldMosaicMode`, `fieldMosaicPanelsX`, `fieldMosaicPanelsY`) work correctly
- They also use `setPlainText()` in `updateMosaicControls()` but don't explicitly set color there
- The difference might be in initialization order or some other subtle difference

**What to Check:**
1. Compare how `fieldMosaicOverlap` is initialized vs. other mosaic controls
2. Check if there's a font size difference in the initial font setting
3. Verify that `setDefaultTextColor()` is the correct method (vs. `setTextColor()` or other methods)
4. Check if the color needs to be set BEFORE `setPlainText()` rather than after
5. Check if there's a font size issue - maybe the font needs to be set in `updateMosaicControls()` as well

### Issue 2: DSO Catalog Version

**Root Cause Hypothesis:**
1. **File Path**: The app might be loading a catalog file from a different location (user data directory vs. app bundle)
2. **Caching**: There might be a cached catalog file with the old version
3. **Multiple Files**: There might be multiple catalog files, and the wrong one is being loaded
4. **Build Issue**: The catalog file in the app bundle might not have been updated

**What to Check:**
1. Verify which catalog file is actually being loaded at runtime
2. Check if there are multiple catalog files (catalog.dat vs. catalog-3.21.dat)
3. Check the user's data directory for a catalog file
4. Verify the catalog file in the app bundle has the correct version
5. Check the file loading logic in `NebulaMgr::loadNebulaSet()` (line 924-952)

---

## Recommended Next Steps

1. **For Issue 1 (Overlap Text):**
   - Set both font AND color in `updateMosaicControls()` for `fieldMosaicOverlap`, matching exactly how other controls work
   - Check if font size needs to be explicitly set in `updateMosaicControls()` as well
   - Consider using the same font object that other controls use
   - Add debug logging to see what color/font values are being set

2. **For Issue 2 (DSO Catalog):**
   - Add debug logging to see which catalog file is being loaded
   - Check the user's data directory for catalog files
   - Verify the catalog file in the app bundle
   - Check if the version string comparison logic is working correctly

3. **General:**
   - Test with a clean build (remove build directory and rebuild)
   - Test with a fresh app installation (remove old app bundle completely)
   - Check console output for any warnings or errors
   - Verify that changes are actually being compiled into the binary

---

## Files Modified

1. `plugins/Oculars/src/gui/OcularsGuiPanel.cpp`
   - Added font setting in constructor (lines 164-174)
   - Added color setting in `setControlsColor()` (line ~1273)
   - Added font setting in `setControlsFont()` (line ~1340)
   - Added color setting in `updateMosaicControls()` (line ~1150)

2. `src/core/modules/NebulaMgr.cpp`
   - Changed `StellariumDSOCatalogVersion` from `"3.20"` to `"3.21"` (line 55)

---

## Build and Installation Commands Used

```bash
cd /Users/ryder/Documents/GitHub/stellarium/build
cmake --build . --target stellarium -j$(sysctl -n hw.ncpu)
rm -rf /Applications/Stellarium.app
cp -R Stellarium.app /Applications/
xattr -cr /Applications/Stellarium.app
find /Applications/Stellarium.app -name "._*" -delete
codesign --force --deep --sign - /Applications/Stellarium.app
```

---

## Verification Needed

1. Confirm the binary was actually rebuilt (check timestamps)
2. Confirm the app bundle was actually replaced (check timestamps)
3. Check if there are any cached files or settings that might be interfering
4. Verify the actual runtime behavior with debug logging

