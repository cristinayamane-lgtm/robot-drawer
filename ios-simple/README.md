# 📱 iOS App - Robot Drawer (Simple Version)

Simple iOS app to control the drawing robot via Bluetooth. No canvas visualization - just buttons and status.

## Requirements

- iOS 14.0+
- Swift 5.5+
- Bluetooth LE capable device

## Features

### 🎮 Controls
- **Square**: Draws a 200mm square
- **Triangle**: Draws an equilateral triangle
- **Pentagon**: Draws a regular pentagon
- **Rectangle**: Customizable length and width
- **Stop**: Stops current drawing
- **Reset**: Resets robot position

### 🔌 Bluetooth
- Automatic HC-06 device discovery
- Simple connection status indicator
- Status messages for each command
- Error handling

### 📊 Status Information
- Connection status (Connected/Disconnected)
- Current operation status
- Simple message display

## Building

1. Open `RobotDrawerSimple.xcodeproj` in Xcode
2. Select your iOS device
3. Press Cmd+R to run

## Usage

### Connecting to HC-06

1. Tap "Scan for Devices"
2. Select "HC-06" from the list
3. Wait for connection confirmation

### Drawing a Shape

1. Tap a shape button (Square, Triangle, Pentagon, Rectangle)
2. Status shows "Drawing..."
3. Wait for "Drawing complete!" message
4. Robô has drawn the shape on paper

## Architecture

### BluetoothManager (MVVM)
- Manages Bluetooth connection
- Sends commands to Arduino
- Parses responses
- Updates UI state

### ContentView (SwiftUI)
- Main user interface
- Device discovery and connection
- Drawing controls
- Status display

## File Structure

```
ios-simple/
├── RobotDrawerSimple/
│   ├── RobotDrawerSimpleApp.swift    # App entry point
│   ├── ContentView.swift             # Main UI
│   ├── BluetoothManager.swift        # Bluetooth logic
│   └── Assets.xcassets               # App icons & images
└── RobotDrawerSimple.xcodeproj/      # Xcode project
```

## Troubleshooting

### "Disconnected" Status
- Check if HC-06 is powered on
- Verify Bluetooth is enabled on iPhone
- Try re-pairing the device
- Check distance between iPhone and robot

### Commands Not Working
- Verify connection is active
- Try "Stop" command
- Restart app

## Notes

- App requires location permission for Bluetooth scanning (iOS privacy requirement)
- HC-06 default PIN: 1234 or 0000
- HC-06 default name: "HC-06"
- Communication runs at 9600 baud
