# Mosaic Planning Plugin Requirements

## Overview

Extend the Oculars plugin to support mosaic planning capabilities, allowing users to visualize N×M mosaic panels with configurable overlap and rotation, similar to NINA's framing assistant or Telescopius's telescope simulator.

## Functional Requirements

### FR1: Mosaic Mode Toggle
- **Description**: User can enable/disable mosaic mode within the Oculars plugin
- **Priority**: High
- **Acceptance Criteria**:
  - Mosaic mode can be toggled independently from single-frame CCD mode
  - Enabling mosaic mode disables single-frame CCD view
  - State persists across sessions
  - Accessible via UI toggle and keyboard shortcut

### FR2: Panel Grid Configuration
- **Description**: User can configure the number of panels in X and Y axes
- **Priority**: High
- **Acceptance Criteria**:
  - X-axis panels: configurable range 1-20 (default: 3)
  - Y-axis panels: configurable range 1-20 (default: 3)
  - Values are positive integers
  - Changes update mosaic display immediately
  - Settings persist across sessions

### FR3: Overlap Percentage Configuration
- **Description**: User can configure overlap percentage between adjacent panels
- **Priority**: High
- **Acceptance Criteria**:
  - Overlap range: 0-50% (default: 20%)
  - Expressed as percentage (0-100)
  - Changes update mosaic display immediately
  - Overlap calculation: panel spacing = FOV × (1 - overlap/100)
  - Settings persist across sessions

### FR4: Rotation Angle Configuration
- **Description**: User can rotate the entire mosaic grid by a specified angle
- **Priority**: High
- **Acceptance Criteria**:
  - Rotation angle: 0-360 degrees (default: 0°)
  - Rotation applied to entire grid pattern
  - Changes update mosaic display immediately
  - Settings persist across sessions
  - Supports decimal precision (e.g., 45.5°)

### FR5: Mosaic Visualization
- **Description**: System displays all mosaic panels as overlapping outlines on the sky
- **Priority**: High
- **Acceptance Criteria**:
  - Each panel displays as rectangular FOV outline
  - All panels use same FOV calculation from selected CCD/Telescope/Lens
  - Panels rendered with proper overlap spacing
  - Grid rotated according to configured rotation angle
  - Mosaic centers on current view or selected object
  - Visible when mosaic mode is enabled

### FR6: Equipment Integration
- **Description**: Mosaic uses existing Oculars equipment data (CCD, Telescope, Lens)
- **Priority**: High
- **Acceptance Criteria**:
  - FOV calculated from currently selected CCD, Telescope, and Lens
  - Changes to equipment selection update mosaic automatically
  - Supports all existing equipment features (reducers, Barlows, binning)
  - No duplicate equipment configuration needed

### FR7: Coordinate Display
- **Description**: Display center coordinates for mosaic grid
- **Priority**: Medium
- **Acceptance Criteria**:
  - Show RA/Dec (J2000.0) of mosaic center
  - Option to show horizontal coordinates (Az/Alt)
  - Display format matches existing Oculars coordinate display
  - Update when mosaic center changes

### FR8: Mosaic Coverage Area
- **Description**: Display total coverage area of the mosaic
- **Priority**: Medium
- **Acceptance Criteria**:
  - Show total width and height in degrees
  - Calculate based on panel count, FOV, and overlap
  - Display format: "W × H" (e.g., "12.5° × 8.3°")
  - Update when configuration changes

### FR9: Panel Numbering/Labeling
- **Description**: Optionally label panels for identification
- **Priority**: Low
- **Acceptance Criteria**:
  - Show panel indices (e.g., "1,1", "1,2", "2,1")
  - Toggle on/off
  - Labels positioned at panel centers
  - Readable when panels are visible

## Technical Requirements

### TR1: Architecture
- **Description**: Extend existing Oculars plugin, do not create separate plugin. Plugin will be built as a dynamic plugin to enable testing with production Stellarium builds.
- **Priority**: High
- **Implementation Notes**:
  - Add mosaic functionality to `plugins/Oculars/src/Oculars.cpp`
  - Reuse existing FOV calculation (`CCD::getActualFOVx/y()`)
  - Reuse existing rendering infrastructure (`drawSensorFrameAndOverlay()`)
  - Build system supports both static (default) and dynamic plugin builds via `BUILD_DYNAMIC_PLUGIN` CMake option
  - Dynamic plugin can be installed to production Stellarium instance for testing

### TR2: Properties and Signals
- **Description**: Expose mosaic configuration via Qt properties
- **Priority**: High
- **Implementation Notes**:
  ```cpp
  Q_PROPERTY(bool flagMosaicMode READ getFlagMosaicMode WRITE setFlagMosaicMode)
  Q_PROPERTY(int mosaicPanelsX READ getMosaicPanelsX WRITE setMosaicPanelsX)
  Q_PROPERTY(int mosaicPanelsY READ getMosaicPanelsY WRITE setMosaicPanelsY)
  Q_PROPERTY(double mosaicRotationAngle READ getMosaicRotationAngle WRITE setMosaicRotationAngle)
  Q_PROPERTY(double mosaicOverlapPercent READ getMosaicOverlapPercent WRITE setMosaicOverlapPercent)
  ```

### TR3: Mosaic Panel Calculation
- **Description**: Calculate panel center positions in sky coordinates
- **Priority**: High
- **Implementation Notes**:
  - Use selected view center (or selected object) as mosaic center
  - Calculate grid positions accounting for overlap
  - Apply rotation to grid
  - Convert to spherical coordinates (RA/Dec)
  - Handle spherical geometry near poles

### TR4: Rendering Performance
- **Description**: Efficient rendering of multiple panels
- **Priority**: Medium
- **Implementation Notes**:
  - Cache panel calculations until parameters change
  - Reuse existing rendering code for each panel
  - Consider LOD for many panels (N×M > 25)
  - Minimize redundant calculations

### TR5: Coordinate Transformations
- **Description**: Accurate sky coordinate transformations
- **Priority**: High
- **Implementation Notes**:
  - Use existing `StelProjector` infrastructure
  - Handle rotation matrices correctly
  - Account for spherical geometry distortion
  - Support both equatorial and alt-az projections

### TR6: UI Integration
- **Description**: Add mosaic controls to Oculars configuration dialog
- **Priority**: High
- **Implementation Notes**:
  - Add "Mosaic" tab to `OcularDialog`
  - Controls: panels X/Y, rotation, overlap
  - Real-time preview updates
  - Integrate with existing `OcularsGuiPanel` (optional)

### TR7: Settings Persistence
- **Description**: Save and restore mosaic configuration
- **Priority**: High
- **Implementation Notes**:
  - Store in Oculars plugin settings (INI file)
  - Load on plugin initialization
  - Save on configuration changes
  - Use existing `QSettings` infrastructure

### TR8: Actions and Shortcuts
- **Description**: Provide keyboard shortcuts for common operations
- **Priority**: Medium
- **Implementation Notes**:
  - Toggle mosaic mode
  - Increment/decrement panel counts
  - Reset rotation angle
  - Use existing `StelAction` infrastructure

## User Interface Requirements

### UI1: Configuration Dialog
- **Location**: Oculars plugin settings dialog
- **Tab**: "Mosaic" or integrate into existing "Sensors" tab
- **Controls**:
  - Checkbox: "Enable mosaic mode"
  - Spinbox: X-axis panels (1-20)
  - Spinbox: Y-axis panels (1-20)
  - Slider/Spinbox: Overlap percentage (0-50%)
  - Slider/Spinbox: Rotation angle (0-360°)
  - Display: Total coverage area (read-only)

### UI2: Floating Panel (Optional)
- **Location**: `OcularsGuiPanel` or similar
- **Controls**:
  - Quick toggle for mosaic mode
  - Panel count adjustments (+/-)
  - Rotation increment/decrement buttons
  - Display current configuration

### UI3: On-Screen Display
- **Location**: Sky viewport overlay
- **Display**:
  - All mosaic panel outlines
  - Panel labels (if enabled)
  - Mosaic center coordinates
  - Total coverage dimensions
  - Panel count and overlap percentage (when space allows)

## Data Model

### DM1: MosaicPanel Structure
```cpp
struct MosaicPanel {
    Vec3d center;           // 3D sky position of panel center
    double rotation;        // Panel rotation in radians
    int panelIndexX;        // Grid X position (0 to panelsX-1)
    int panelIndexY;        // Grid Y position (0 to panelsY-1)
    QRect boundingRect;    // Screen-space bounding rect (optional, for optimization)
};
```

### DM2: Configuration Storage
- **Format**: INI file (existing Oculars settings)
- **Keys**:
  - `mosaic_mode_enabled` (bool)
  - `mosaic_panels_x` (int)
  - `mosaic_panels_y` (int)
  - `mosaic_rotation_angle` (double)
  - `mosaic_overlap_percent` (double)

## Performance Requirements

### PR1: Rendering Performance
- **Target**: Render up to 100 panels (10×10) at 60 FPS
- **Optimization**: Cache panel positions, simplify rendering for off-screen panels

### PR2: Calculation Performance
- **Target**: Recalculate panel positions in < 16ms when parameters change
- **Optimization**: Cache calculations, only recompute on parameter change

### PR3: Memory Usage
- **Target**: Minimal overhead (< 1MB for typical configurations)
- **Optimization**: Use stack allocation where possible, cache efficiently

## Testing Requirements

### Test1: Panel Calculation
- Verify correct panel spacing with different overlap percentages
- Verify rotation applied correctly to grid
- Test edge cases: 1×1 mosaic, large grids (20×20)

### Test2: Coordinate Accuracy
- Verify panel centers calculated correctly
- Test near equator vs near poles (spherical geometry)
- Verify rotation matrices produce correct results

### Test3: Equipment Integration
- Verify FOV calculated correctly for all equipment combinations
- Test with reducers, Barlows, different sensors
- Verify updates when equipment selection changes

### Test4: UI Functionality
- Test all configuration controls work correctly
- Verify settings persist across sessions
- Test keyboard shortcuts

### Test5: Visual Verification
- Visual inspection of mosaic overlays
- Compare with expected results from manual calculations
- Test at various zoom levels and sky positions

## Compatibility Requirements

### CMP1: Backward Compatibility
- Existing Oculars functionality must remain unchanged
- Single-frame CCD mode must continue to work
- No changes to existing equipment data structure

### CMP2: Platform Compatibility
- Support all platforms Stellarium supports (Linux, macOS, Windows)
- No platform-specific code unless necessary

## Documentation Requirements

### DOC1: User Documentation
- Update Oculars plugin documentation
- Add mosaic planning usage guide
- Include examples and use cases

### DOC2: Developer Documentation
- Document mosaic calculation algorithm
- Code comments for complex calculations
- Update plugin architecture documentation

## Future Enhancements (Out of Scope)

- Filter color coding (removed per requirements)
- Export mosaic coordinates to file
- Integration with telescope control software
- Automatic mosaic sequence planning
- Panel capture progress tracking

## Dependencies

### Internal
- Oculars plugin (`plugins/Oculars/`)
- StelCore, StelProjector, StelPainter (core rendering)
- CCD, Telescope, Lens classes (equipment data)

### External
- Qt 5.x (for UI and properties)
- Standard C++ library

## Build and Deployment

### Build Configuration
- **Static Plugin (Default)**: Built into main Stellarium binary
  - Standard build: `cmake .. && make`
  - No special configuration needed
  
- **Dynamic Plugin (For Testing)**: Built as separate `.so`/`.dylib`/`.dll`
  - Build with: `cmake -DBUILD_DYNAMIC_PLUGIN=ON .. && make`
  - Output: `modules/Oculars/Oculars.dylib` (macOS) or equivalent
  - Can be installed to production Stellarium instance for testing
  - Installation path: `~/.stellarium/modules/Oculars/` (Linux/macOS) or equivalent user data directory

### Testing with Production Stellarium
1. Build Oculars plugin as dynamic plugin: `cmake -DBUILD_DYNAMIC_PLUGIN=ON .. && make`
2. Locate production Stellarium user data directory:
   - macOS: `~/Library/Application Support/Stellarium/`
   - Linux: `~/.stellarium/`
   - Windows: `%APPDATA%\Stellarium\`
3. Create `modules/Oculars/` directory if it doesn't exist
4. Copy built plugin library and resource files to the directory
5. Restart Stellarium to load the modified plugin

## Risk Assessment

### Risk1: Spherical Geometry Complexity
- **Impact**: High - Panels may display incorrectly near poles
- **Mitigation**: Thorough testing, consider great circle calculations

### Risk2: Performance with Large Grids
- **Impact**: Medium - Many panels may affect frame rate
- **Mitigation**: Implement LOD, optimize rendering

### Risk3: Coordinate Transformation Accuracy
- **Impact**: High - Incorrect panel positions
- **Mitigation**: Reuse proven transformation code, extensive testing

## Success Criteria

1. ✅ User can configure and visualize N×M mosaic with overlap and rotation
2. ✅ Mosaic displays correctly aligned with sky coordinates
3. ✅ Performance acceptable for typical use cases (up to 10×10 panels)
4. ✅ Integration with existing Oculars equipment seamless
5. ✅ Settings persist correctly across sessions
6. ✅ No regression in existing Oculars functionality

## Implementation Phases

### Phase 1: Core Functionality
- Mosaic mode toggle
- Panel grid calculation
- Basic rendering (single color for all panels)
- Equipment integration

### Phase 2: Configuration UI
- Configuration dialog integration
- Settings persistence
- Real-time updates

### Phase 3: Enhanced Display
- Coordinate display
- Coverage area display
- Panel labeling (optional)
- Performance optimization

### Phase 4: Polish
- Keyboard shortcuts
- Floating panel integration (optional)
- Documentation
- Testing and bug fixes

## Notes

- All FOV calculations use existing `CCD::getActualFOVx/y()` methods
- No changes needed to equipment data structures
- Filter support removed from scope (filters don't change FOV)
- Focus on extending existing code rather than creating new infrastructure

