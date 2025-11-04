# MosaicPlanner Plugin - Usage Guide

## Quick Start

The plugin loads but currently has **no GUI controls**. You control it via the **Scripting Console**.

## Using the Scripting Console

### Step 1: Open Scripting Console

**Method 1**: Press `~` (tilde key)  
**Method 2**: Press `F12`  
**Method 3**: Window menu → Scripting Console

### Step 2: Enable Mosaic Mode

In the scripting console, type:

```javascript
MosaicPlanner.flagMosaicMode = true;
```

Press Enter. You should see yellow mosaic panel outlines appear on the sky!

### Step 3: Configure Mosaic

```javascript
// Set number of panels (default: 3x3)
MosaicPlanner.mosaicPanelsX = 5;  // 5 panels horizontally
MosaicPlanner.mosaicPanelsY = 3;   // 3 panels vertically

// Set overlap percentage (default: 20%)
MosaicPlanner.mosaicOverlapPercent = 25.0;  // 25% overlap

// Set rotation angle (default: 0 degrees)
MosaicPlanner.mosaicRotationAngle = 45.0;  // Rotate 45 degrees
```

### Step 4: Disable Mosaic Mode

```javascript
MosaicPlanner.flagMosaicMode = false;
```

## Available Properties

All properties can be accessed via scripting:

| Property | Type | Range | Default | Description |
|----------|------|-------|---------|-------------|
| `flagMosaicMode` | boolean | true/false | false | Enable/disable mosaic visualization |
| `mosaicPanelsX` | int | 1-20 | 3 | Number of panels horizontally |
| `mosaicPanelsY` | int | 1-20 | 3 | Number of panels vertically |
| `mosaicRotationAngle` | double | 0-360 | 0.0 | Grid rotation in degrees |
| `mosaicOverlapPercent` | double | 0-50 | 20.0 | Overlap percentage between panels |

## Example Usage

### 3x3 Grid with 20% Overlap
```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 3;
MosaicPlanner.mosaicPanelsY = 3;
MosaicPlanner.mosaicOverlapPercent = 20.0;
MosaicPlanner.mosaicRotationAngle = 0.0;
```

### 5x2 Grid with 30% Overlap, Rotated 45°
```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 5;
MosaicPlanner.mosaicPanelsY = 2;
MosaicPlanner.mosaicOverlapPercent = 30.0;
MosaicPlanner.mosaicRotationAngle = 45.0;
```

### Large 10x10 Grid
```javascript
MosaicPlanner.flagMosaicMode = true;
MosaicPlanner.mosaicPanelsX = 10;
MosaicPlanner.mosaicPanelsY = 10;
MosaicPlanner.mosaicOverlapPercent = 15.0;
```

## Equipment Configuration

The plugin uses equipment from the **Oculars plugin**:

1. **Configure Equipment**:
   - Configuration → Plugins → Oculars → Configure
   - Add CCD (camera sensor)
   - Add Telescope
   - Add Lens (optional - reducer/Barlow)

2. **MosaicPlanner Uses Same Equipment**:
   - The plugin automatically uses the first CCD and Telescope
   - Mosaic panel size is calculated from your equipment's FOV

## Reading Current Values

You can check current settings:

```javascript
// Check if mosaic mode is enabled
MosaicPlanner.flagMosaicMode

// Check current panel counts
MosaicPlanner.mosaicPanelsX
MosaicPlanner.mosaicPanelsY

// Check current settings
MosaicPlanner.mosaicOverlapPercent
MosaicPlanner.mosaicRotationAngle
```

## Tips

1. **Center the View**: The mosaic is centered on your current view. Pan to where you want the mosaic, then enable it.

2. **Adjust for Equipment**: The panel size depends on your selected CCD/Telescope/Lens. Make sure equipment is configured in Oculars first.

3. **Rotation**: Use rotation to align the mosaic with your target's orientation.

4. **Overlap**: Higher overlap (30-40%) gives more margin for alignment, but requires more panels to cover the same area.

## Troubleshooting

### Mosaic Not Displaying

1. **Check Mosaic Mode**:
   ```javascript
   MosaicPlanner.flagMosaicMode  // Should be true
   ```

2. **Check Equipment**:
   - Ensure CCD and Telescope are configured in Oculars
   - MosaicPlanner uses the first CCD and Telescope

3. **Check Settings**:
   ```javascript
   MosaicPlanner.mosaicPanelsX  // Should be > 0
   MosaicPlanner.mosaicPanelsY  // Should be > 0
   ```

### Panels Appear Wrong

- Check equipment FOV settings in Oculars
- Verify CCD sensor size matches your camera
- Check telescope focal length is correct

## Next: GUI Controls

A GUI configuration dialog is planned to make this easier to use. For now, scripting is the only interface.

