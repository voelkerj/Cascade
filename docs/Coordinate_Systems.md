# Coordinate Systems
When interacting with Cascade, the user generally provides entity locations in WCS, as well as a Camera position, zoom, and view angle. Cascade automatically converts entity coordinates to SDL for drawing to the window.

### World Coordinate System (WCS)
- **Origin**: Center of World
- **Units**: World units (arbitrary)
- **+X**: Right
- **+Y**: Up
- **Rotation**: Rotated about the x-axis by the view angle

### Unrotated WCS (uWCS)
- **Origin**: Center of World
- **Units**: World units (arbitrary). Camera zoom is ratio of pixels to World units.
- **+X**: Right
- **+Y**: Up
- **Rotation**: None

### Pixel Coordinate System (PCS)
- **Origin**: Center of Window
- **Units**: Pixels
- **+X**: Right
- **+Y**: Up
- **Rotation**: None

### SDL Coordinate System 
- **Origin**: Top Left Corner of Window
- **Units**: Pixels
- **+X**: Right
- **+Y**: Down
- **Rotation**: None

### Hex Coordinate System
- **Origin**: Center of World
- **Units**: Hexes
- **+X**: N/A
- **+Y**: N/A
- **Rotation**: None

## Conversions
Helper functions are provided to convert between coordinate systems:
- WCS2uWCS / uWCS2WCS
- PCS2WCS / WCS2PCS
- PCS2SDL / SDL2PCS
- HEX2WCS / WCS2HEX