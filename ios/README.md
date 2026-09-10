# 📱 iOS App - Robot Drawer

Controls the drawing robot via Bluetooth from iPhone.

## Requirements

- iOS 14.0+
- Swift 5.5+
- Bluetooth LE capable device

## Building

1. Open `RobotDrawer.xcodeproj` in Xcode
2. Select your iOS device or simulator
3. Press Cmd+R to run

## Features

### 🎮 Controls
- **Square**: Draws a 200mm square
- **Triangle**: Draws an equilateral triangle
- **Pentagon**: Draws a regular pentagon
- **Rectangle**: Customizable length and width
- **Stop**: Stops current drawing
- **Reset**: Resets robot position

### 📊 Canvas Display
- Real-time visualization of robot's drawing path
- Grid overlay for reference
- Green dot: Starting position
- Red dot: Current position
- Red line: Drawing path

### 📡 Bluetooth Communication
- Automatic HC-06 device discovery
- Real-time status updates (position, pen state)
- Error handling with user feedback
- Checksum validation for data integrity

### 📈 Status Information
- Current X, Y coordinates
- Pen up/down state
- Drawing duration
- Connection status

## Bluetooth Protocol

See `../docs/BLUETOOTH_PROTOCOL.md` for detailed protocol specification.

## Architecture

### BluetoothManager (MVVM)
- Manages Bluetooth connection lifecycle
- Handles frame parsing and validation
- Sends commands to Arduino
- Updates UI with real-time status

### ContentView (SwiftUI)
- Main user interface
- Device discovery and connection
- Drawing controls
- Real-time canvas visualization

### CanvasView (SwiftUI Canvas)
- Renders drawing path in real-time
- Grid display
- Position indicators

## File Structure

```
ios/
├── RobotDrawer/
│   ├── RobotDrawerApp.swift      # App entry point
│   ├── ContentView.swift         # Main UI
│   ├── BluetoothManager.swift    # Bluetooth logic
│   ├── Extensions.swift          # Utility extensions
│   └── Assets.xcassets           # App icons & images
└── RobotDrawer.xcodeproj/        # Xcode project
```

## Usage

### Connecting to HC-06

1. Ensure HC-06 is powered on and paired
2. Tap "Scan for Devices"
3. Select "HC-06" from the list
4. Wait for connection confirmation

### Drawing a Shape

1. Tap a shape button (Square, Triangle, Pentagon, Rectangle)
2. Watch the canvas as the robot draws
3. Monitor position and pen state in status area
4. Wait for "Drawing complete" message

### Customizing Rectangle

1. Use Length (L) slider: 10-50 cm
2. Use Width (W) slider: 5-30 cm
3. Tap "Draw" button

## Troubleshooting

### "Disconnected" Status
- Check if HC-06 is powered on
- Verify Bluetooth is enabled on iPhone
- Try re-pairing the device
- Check distance between iPhone and robot

### "Error: Checksum"
- Indicates data corruption
- Check HC-06 connection quality
- Verify baud rate (should be 9600)
- Try resetting both devices

### Canvas Not Updating
- Check robot is drawing (should see motors moving)
- Verify Bluetooth connection is active
- Try requesting status manually
- Check position values in status area

## Notes

- App requires location permission for Bluetooth scanning (iOS privacy requirement)
- HC-06 default PIN: 1234 or 0000
- HC-06 default name: "HC-06"
- Communication runs at 9600 baud (configurable via Arduino code)
