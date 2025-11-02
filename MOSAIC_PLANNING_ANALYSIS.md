# Mosaic Planning Plugin Analysis for Stellarium

## Executive Summary

This document analyzes the Stellarium codebase for developing a mosaic planning plugin that extends the Oculars plugin functionality. The goal is to provide capabilities similar to NINA's framing assistant or Telescopius's telescope simulator, allowing users to visualize N×M mosaic panels with configurable overlap and rotation.

## Current State Analysis

### 1. Oculars Plugin Architecture

**Location**: `plugins/Oculars/`

**Key Components**:
- **Oculars.hpp/cpp**: Main plugin class extending `StelModule`
- **CCD.hpp/cpp**: Represents camera sensors with properties:
  - Resolution (X/Y pixels)
  - Chip dimensions (width/height in mm)
  - Rotation angle
  - Binning factors
  - Off-axis guider support
- **Telescope.hpp/cpp**: Telescope configurations (focal length, diameter, mount type)
- **Lens.hpp/cpp**: Reducer/Barlow multipliers
- **Ocular.hpp/cpp**: Eyepiece configurations

**FOV Calculation**:
The plugin calculates actual field of view using:
```cpp
double CCD::getActualFOVx(const Telescope *telescope, const Lens *lens) const
{
    const double lens_multipler = (lens != Q_NULLPTR ? lens->getMultipler() : 1.0);
    double fovX = RADIAN_TO_DEGREES * 2 * qAtan(this->chipWidth() / (2.0 * telescope->focalLength() * lens_multipler));
    return fovX;
}
```

**Rendering Approach** (`paintCCDBounds()` in `Oculars.cpp:1826-2071`):
1. Gets current view center (screen center or selected object)
2. Calculates FOV using selected CCD, telescope, and lens
3. Creates rotation matrices based on:
   - Azimuth/elevation of view center
   - CCD chip rotation angle
4. Projects FOV rectangle onto sky using `drawSensorFrameAndOverlay()`
5. Renders frame using `StelPainter` with `LineLoop` drawing

**Key Rendering Code** (`drawSensorFrameAndOverlay()` in `Oculars.cpp:1595-1757`):
- Uses tangent-based projection to handle wide FOV
- Projects 3D vectors onto screen coordinates
- Handles rotation through matrix transformations (`derotate`)
- Supports crop overlays and pixel grids

### 2. Existing MosaicCamera Plugin

**Location**: `plugins/MosaicCamera/`

**Current Functionality**:
- Loads predefined polygon sets from JSON files
- Displays multiple camera overlays on the sky
- Each camera has RA/Dec position and rotation
- **Limitation**: Does NOT integrate with Oculars equipment data
- **Limitation**: Does NOT dynamically generate mosaics based on panel counts
- **Limitation**: Uses pre-defined polygon sets rather than calculating from sensor/telescope parameters

**Conclusion**: While MosaicCamera exists, it does not provide the functionality you need. It's more for displaying predefined camera footprints rather than planning mosaics dynamically.

### 3. Plugin Architecture

**Plugin Interface**:
- All plugins extend `StelModule` and implement `StelPluginInterface`
- Plugins register via `StelModuleMgr::registerModule()`
- Static plugins are linked at build time
- Dynamic plugins are loaded from `modules/` directory
- Plugin structure requires:
  - Main class extending `StelModule`
  - Interface class extending `QObject, StelPluginInterface`
  - `CMakeLists.txt` for build configuration
  - Resource file (`.qrc`) for UI assets

**Core Services Used**:
- `StelCore`: Provides projections (equatorial, alt-az)
- `StelPainter`: OpenGL-based drawing
- `StelProjector`: Handles coordinate transformations
- `StelApp`: Application singleton for settings, actions
- `StelGui`: UI management

## Implementation Recommendations

### Option 1: Extend Oculars Plugin (Recommended)

**Advantages**:
- Reuses existing equipment data (CCD, Telescope, Lens)
- Can reuse FOV calculation logic
- Natural extension of existing functionality
- Users already familiar with Oculars interface

**Implementation Plan**:

1. **Add Mosaic Mode Toggle**:
   - New property: `flagMosaicMode` (bool)
   - When enabled, renders mosaic grid instead of single frame
   - Integrate into existing CCD mode toggle

2. **Mosaic Configuration Properties**:
   ```cpp
   Q_PROPERTY(int mosaicPanelsX READ getMosaicPanelsX WRITE setMosaicPanelsX)
   Q_PROPERTY(int mosaicPanelsY READ getMosaicPanelsY WRITE setMosaicPanelsY)
   Q_PROPERTY(double mosaicRotationAngle READ getMosaicRotationAngle WRITE setMosaicRotationAngle)
   Q_PROPERTY(double mosaicOverlapPercent READ getMosaicOverlapPercent WRITE setMosaicOverlapPercent)
   ```

3. **Core Mosaic Calculation Logic**:
   ```cpp
   struct MosaicPanel {
       Vec3d center;        // 3D sky position of panel center
       double rotation;    // Panel rotation in radians
       int panelIndexX;    // Panel position in grid (0 to panelsX-1)
       int panelIndexY;    // Panel position in grid (0 to panelsY-1)
   };
   
   QVector<MosaicPanel> calculateMosaicPanels() {
       // 1. Get base FOV from selected CCD/Telescope/Lens
       // 2. Calculate effective FOV accounting for overlap
       // 3. Generate grid of panel centers
       // 4. Apply rotation to grid
       // 5. Transform centers to sky coordinates
       // 6. Return panel list
   }
   ```

4. **Rendering Function**:
   ```cpp
   void paintMosaicBounds() {
       QVector<MosaicPanel> panels = calculateMosaicPanels();
       
       for (const auto& panel : panels) {
           // Similar to paintCCDBounds() but:
           // - Use panel.center instead of view center
           // - Add panel rotation to rotation matrix
           // - Optionally use different color per panel
           drawMosaicPanelFrame(panel);
       }
   }
   ```

5. **Overlap Calculation**:
   - If overlap = 20%, each panel moves 80% of FOV distance
   - Example: For 3×3 mosaic with 20% overlap:
     - Effective panel spacing = FOV × 0.8
     - Total coverage = FOV × (2 + panels × 0.8)

### Option 2: Create New Plugin (Alternative)

**Advantages**:
- Cleaner separation of concerns
- Won't bloat Oculars plugin
- Can be developed independently

**Disadvantages**:
- Need to duplicate or access Oculars equipment data
- Users must configure equipment in two places
- More complex data sharing

**If choosing this option**:
- Consider accessing Oculars plugin data via `StelModuleMgr::getModule("Oculars")`
- Or reimplement equipment configuration (less ideal)

## Technical Details

### Coordinate Transformations

The Oculars plugin uses these key transformations:

1. **View Center to Sky Position**:
   ```cpp
   Vec3d centerPos3d;
   projector->unProject(centerScreen[0], centerScreen[1], centerPos3d);
   // centerPos3d now contains 3D sky position
   ```

2. **Rotation Matrix Construction**:
   ```cpp
   Mat4f derotate = Mat4f::rotation(Vec3f(0,0,1), azimuth) *
                    Mat4f::rotation(Vec3f(0,1,0), -elevation) *
                    Mat4f::rotation(Vec3f(1,0,0), rotationAngle * (M_PI/180));
   ```

3. **FOV Projection**:
   ```cpp
   // For each edge point of FOV rectangle:
   Vec3f skyPoint = derotate * Vec3f(1, tanFovX, tanFovY);
   Vec3f screenPoint;
   projector->project(skyPoint, screenPoint);
   ```

### Mosaic Panel Position Calculation

For an N×M mosaic with R rotation and O overlap percent:

1. **Calculate base FOV**: From selected CCD/Telescope/Lens
2. **Calculate step size**: `step = FOV × (1 - overlap/100)`
3. **Calculate grid extent**: 
   - Total width = `(N-1) × step + FOV`
   - Total height = `(M-1) × step + FOV`
4. **Generate grid centers**:
   ```cpp
   for (int i = 0; i < N; i++) {
       for (int j = 0; j < M; j++) {
           // Local grid position (centered)
           double localX = (i - (N-1)/2.0) * step;
           double localY = (j - (M-1)/2.0) * step;
           
           // Apply rotation
           double rotatedX = localX * cos(R) - localY * sin(R);
           double rotatedY = localX * sin(R) + localY * cos(R);
           
           // Convert to sky coordinates (spherical)
           // This requires converting angular offsets to RA/Dec offsets
       }
   }
   ```

5. **Sky Coordinate Conversion**:
   - Angular offsets at celestial equator are straightforward
   - Near poles, need to account for spherical geometry
   - Use `Vec3d::sphericalToRectangular()` and matrix transformations

### UI Integration Points

**Configuration Dialog** (`OcularDialog.hpp/cpp`):
- Add "Mosaic" tab to existing dialog
- Controls for: panels X/Y, rotation angle, overlap percent
- Preview checkbox

**Gui Panel** (`OcularsGuiPanel.hpp/cpp`):
- Add mosaic controls to floating panel
- Real-time adjustment of mosaic parameters
- Display total coverage area

**Actions**:
```cpp
addAction("actionToggle_Mosaic_Mode", "Oculars", "Toggle mosaic mode", "flagMosaicMode");
addAction("actionMosaic_PanelsX_Inc", "Oculars", "Increase X panels", "incrementMosaicPanelsX()");
addAction("actionMosaic_PanelsY_Inc", "Oculars", "Increase Y panels", "incrementMosaicPanelsY()");
```

## Key Files to Modify/Extend

### If Extending Oculars:

1. **`plugins/Oculars/src/Oculars.hpp`**:
   - Add mosaic properties
   - Add `paintMosaicBounds()` method declaration
   - Add `calculateMosaicPanels()` helper

2. **`plugins/Oculars/src/Oculars.cpp`**:
   - Implement `paintMosaicBounds()`
   - Modify `draw()` to call mosaic rendering when enabled
   - Implement mosaic calculation logic

3. **`plugins/Oculars/src/gui/OcularDialog.hpp/cpp`**:
   - Add mosaic configuration UI
   - Connect controls to properties

4. **`plugins/Oculars/resources/default_ocular.ini`**:
   - Add default mosaic settings (if storing in ini)

### If Creating New Plugin:

1. Create new directory: `plugins/MosaicPlanner/`
2. Follow structure similar to `HelloStelModule/` or `AngleMeasure/`
3. Access Oculars data via:
   ```cpp
   Oculars* oculars = GETSTELMODULE(Oculars);
   // Access equipment data
   ```

## Challenges and Considerations

1. **Spherical Geometry**: 
   - Grid spacing needs to account for spherical coordinates
   - Panels near poles will have distorted shapes
   - May need to use great circle distances

2. **Performance**:
   - Rendering N×M panels could be expensive
   - Consider LOD: simplify rendering when panels are small on screen
   - Cache panel calculations until parameters change

3. **Integration with Oculars**:
   - Mosaic mode should probably disable single-frame CCD mode
   - Need to decide: replace or complement existing CCD view?

4. **Coordinate Systems**:
   - Should mosaic center on current view or selected object?
   - Allow user to manually set mosaic center RA/Dec
   - Sync with telescope control if available

## Example Implementation Structure

```cpp
// In Oculars.hpp
class Oculars : public StelModule {
    // ... existing code ...
    
    // Mosaic properties
    Q_PROPERTY(bool flagMosaicMode READ getFlagMosaicMode WRITE setFlagMosaicMode)
    Q_PROPERTY(int mosaicPanelsX READ getMosaicPanelsX WRITE setMosaicPanelsX)
    Q_PROPERTY(int mosaicPanelsY READ getMosaicPanelsY WRITE setMosaicPanelsY)
    Q_PROPERTY(double mosaicRotationAngle READ getMosaicRotationAngle WRITE setMosaicRotationAngle)
    Q_PROPERTY(double mosaicOverlapPercent READ getMosaicOverlapPercent WRITE setMosaicOverlapPercent)
    
private:
    void paintMosaicBounds();
    QVector<MosaicPanel> calculateMosaicPanels();
    void drawMosaicPanelFrame(const MosaicPanel& panel);
    
    bool flagMosaicMode;
    int mosaicPanelsX, mosaicPanelsY;
    double mosaicRotationAngle;  // degrees
    double mosaicOverlapPercent;
    QVector<MosaicPanel> cachedPanels;  // Cache until params change
};
```

## Testing Strategy

1. **Unit Tests**:
   - Mosaic panel calculation logic
   - Overlap percentage verification
   - Rotation matrix calculations

2. **Visual Tests**:
   - 2×2, 3×3, 5×7 mosaics at different overlaps
   - Rotation at 0°, 45°, 90°
   - Near equator vs near poles
   - With/without reducer/Barlow

3. **Integration Tests**:
   - Switching between single frame and mosaic mode
   - Equipment selection changes update mosaic
   - Settings persistence

## References

- **Oculars Plugin**: `plugins/Oculars/`
- **MosaicCamera Plugin**: `plugins/MosaicCamera/` (reference, but different approach)
- **Stellarium Plugin Documentation**: https://stellarium.org/doc-plugins/0.14/
- **Core API Documentation**: https://stellarium.org/doc/25.0/
- **Key Rendering Code**: `plugins/Oculars/src/Oculars.cpp:1595-1757` (drawSensorFrameAndOverlay)
- **FOV Calculation**: `plugins/Oculars/src/CCD.cpp:239-251`

## Next Steps

1. **Decide**: Extend Oculars or create new plugin?
2. **Prototype**: Implement basic N×M grid calculation
3. **Rendering**: Adapt `drawSensorFrameAndOverlay()` for multiple panels
4. **UI**: Add configuration controls
5. **Testing**: Verify with real equipment configurations

