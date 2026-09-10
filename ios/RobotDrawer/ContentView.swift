import SwiftUI

struct ContentView: View {
    @StateObject private var bluetoothManager = BluetoothManager()
    @State private var showDeviceList = false
    @State private var selectedLength: Double = 20
    @State private var selectedWidth: Double = 10
    
    var body: some View {
        ZStack {
            // Background
            LinearGradient(
                gradient: Gradient(colors: [
                    Color(red: 0.1, green: 0.1, blue: 0.2),
                    Color(red: 0.2, green: 0.1, blue: 0.3)
                ]),
                startPoint: .topLeadingAngle,
                endPoint: .bottomTrailingAngle
            )
            .ignoresSafeArea()
            
            VStack(spacing: 20) {
                // Header
                VStack(spacing: 8) {
                    Text("🤖 ROBOT DRAWER")
                        .font(.title)
                        .fontWeight(.bold)
                        .foregroundColor(.white)
                    
                    HStack(spacing: 10) {
                        Circle()
                            .fill(bluetoothManager.isConnected ? Color.green : Color.red)
                            .frame(width: 12, height: 12)
                        
                        Text(bluetoothManager.connectionStatus)
                            .font(.caption)
                            .foregroundColor(.white)
                        
                        Spacer()
                    }
                    .padding(.horizontal)
                }
                .padding()
                .background(Color.black.opacity(0.3))
                .cornerRadius(12)
                .padding()
                
                // Canvas
                CanvasView(drawingPath: bluetoothManager.drawingPath)
                    .frame(height: 300)
                    .background(Color.white)
                    .cornerRadius(12)
                    .padding()
                    .shadow(radius: 5)
                
                // Status Info
                VStack(alignment: .leading, spacing: 8) {
                    HStack {
                        Text("Position:")
                            .font(.caption)
                            .foregroundColor(.gray)
                        Text("X: \(bluetoothManager.robotStatus.x) mm, Y: \(bluetoothManager.robotStatus.y) mm")
                            .font(.caption)
                            .foregroundColor(.white)
                    }
                    
                    HStack {
                        Text("Pen:")
                            .font(.caption)
                            .foregroundColor(.gray)
                        Text(bluetoothManager.robotStatus.penDown ? "DOWN ⬇️" : "UP ⬆️")
                            .font(.caption)
                            .foregroundColor(bluetoothManager.robotStatus.penDown ? .red : .green)
                    }
                    
                    HStack {
                        Text("Status:")
                            .font(.caption)
                            .foregroundColor(.gray)
                        Text(bluetoothManager.robotStatus.lastMessage)
                            .font(.caption)
                            .foregroundColor(.white)
                            .lineLimit(1)
                    }
                }
                .padding()
                .background(Color.black.opacity(0.3))
                .cornerRadius(12)
                .padding(.horizontal)
                
                // Control Buttons
                if bluetoothManager.isConnected {
                    VStack(spacing: 12) {
                        // Row 1: Basic Shapes
                        HStack(spacing: 12) {
                            ShapeButton(
                                title: "⬜ Square",
                                color: Color.blue,
                                action: { bluetoothManager.drawSquare() }
                            )
                            
                            ShapeButton(
                                title: "📐 Triangle",
                                color: Color.purple,
                                action: { bluetoothManager.drawTriangle() }
                            )
                        }
                        
                        // Row 2: Pentagon & Rectangle
                        HStack(spacing: 12) {
                            ShapeButton(
                                title: "⬠ Pentagon",
                                color: Color.orange,
                                action: { bluetoothManager.drawPentagon() }
                            )
                            
                            VStack(spacing: 8) {
                                Text("Rectangle")
                                    .font(.caption)
                                    .fontWeight(.semibold)
                                    .foregroundColor(.white)
                                
                                HStack(spacing: 8) {
                                    VStack(spacing: 4) {
                                        Text("L")
                                            .font(.caption2)
                                            .foregroundColor(.gray)
                                        Slider(value: $selectedLength, in: 10...50, step: 5)
                                            .frame(maxHeight: 30)
                                        Text(String(format: "%.0f cm", selectedLength))
                                            .font(.caption2)
                                            .foregroundColor(.white)
                                    }
                                    
                                    VStack(spacing: 4) {
                                        Text("W")
                                            .font(.caption2)
                                            .foregroundColor(.gray)
                                        Slider(value: $selectedWidth, in: 5...30, step: 5)
                                            .frame(maxHeight: 30)
                                        Text(String(format: "%.0f cm", selectedWidth))
                                            .font(.caption2)
                                            .foregroundColor(.white)
                                    }
                                }
                                
                                Button(action: {
                                    bluetoothManager.drawRectangle(
                                        length: UInt8(selectedLength),
                                        width: UInt8(selectedWidth)
                                    )
                                }) {
                                    Text("Draw")
                                        .font(.caption)
                                        .fontWeight(.semibold)
                                        .foregroundColor(.white)
                                        .frame(maxWidth: .infinity)
                                        .padding(8)
                                        .background(Color.cyan)
                                        .cornerRadius(8)
                                }
                            }
                            .padding()
                            .background(Color.black.opacity(0.3))
                            .cornerRadius(12)
                        }
                        
                        // Row 3: Control Buttons
                        HStack(spacing: 12) {
                            Button(action: { bluetoothManager.reset() }) {
                                Label("Reset", systemImage: "arrow.counterclockwise")
                                    .font(.caption)
                                    .fontWeight(.semibold)
                                    .foregroundColor(.white)
                                    .frame(maxWidth: .infinity)
                                    .padding(10)
                                    .background(Color.yellow)
                                    .cornerRadius(8)
                            }
                            
                            Button(action: { bluetoothManager.stop() }) {
                                Label("Stop", systemImage: "stop.fill")
                                    .font(.caption)
                                    .fontWeight(.semibold)
                                    .foregroundColor(.white)
                                    .frame(maxWidth: .infinity)
                                    .padding(10)
                                    .background(Color.red)
                                    .cornerRadius(8)
                            }
                        }
                    }
                    .padding()
                    .background(Color.black.opacity(0.3))
                    .cornerRadius(12)
                    .padding(.horizontal)
                } else {
                    // Connection Button
                    VStack(spacing: 12) {
                        if bluetoothManager.isScanning {
                            HStack(spacing: 8) {
                                ProgressView()
                                    .tint(.white)
                                Text("Scanning for HC-06...")
                                    .foregroundColor(.white)
                                Spacer()
                            }
                            .padding()
                            .background(Color.black.opacity(0.3))
                            .cornerRadius(12)
                        }
                        
                        if !bluetoothManager.availableDevices.isEmpty {
                            VStack(alignment: .leading, spacing: 8) {
                                Text("Available Devices:")
                                    .font(.caption)
                                    .fontWeight(.semibold)
                                    .foregroundColor(.white)
                                
                                ForEach(bluetoothManager.availableDevices, id: \.identifier) { device in
                                    Button(action: {
                                        bluetoothManager.connect(to: device)
                                    }) {
                                        HStack {
                                            Image(systemName: "bluetooth")
                                            Text(device.name ?? "Unknown")
                                            Spacer()
                                            Image(systemName: "chevron.right")
                                        }
                                        .font(.caption)
                                        .foregroundColor(.white)
                                        .frame(maxWidth: .infinity)
                                        .padding(10)
                                        .background(Color.blue.opacity(0.6))
                                        .cornerRadius(8)
                                    }
                                }
                            }
                            .padding()
                            .background(Color.black.opacity(0.3))
                            .cornerRadius(12)
                        }
                        
                        Button(action: {
                            if bluetoothManager.isScanning {
                                bluetoothManager.stopScanning()
                            } else {
                                bluetoothManager.startScanning()
                            }
                        }) {
                            HStack {
                                Image(systemName: "magnifyingglass")
                                Text(bluetoothManager.isScanning ? "Stop Scanning" : "Scan for Devices")
                                Spacer()
                            }
                            .font(.caption)
                            .fontWeight(.semibold)
                            .foregroundColor(.white)
                            .frame(maxWidth: .infinity)
                            .padding(12)
                            .background(Color.green)
                            .cornerRadius(8)
                        }
                    }
                    .padding()
                    .background(Color.black.opacity(0.3))
                    .cornerRadius(12)
                    .padding(.horizontal)
                }
                
                Spacer()
            }
            .padding(.vertical)
        }
    }
}

// MARK: - Shape Button Component

struct ShapeButton: View {
    let title: String
    let color: Color
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            Text(title)
                .font(.caption)
                .fontWeight(.semibold)
                .foregroundColor(.white)
                .frame(maxWidth: .infinity)
                .padding(12)
                .background(color)
                .cornerRadius(8)
        }
    }
}

// MARK: - Canvas View

struct CanvasView: View {
    let drawingPath: [CGPoint]
    
    var body: some View {
        Canvas { context in
            // Draw grid
            var path = Path()
            for i in stride(from: 0, through: 300, by: 30) {
                path.move(to: CGPoint(x: i, y: 0))
                path.addLine(to: CGPoint(x: i, y: 400))
                path.move(to: CGPoint(x: 0, y: i))
                path.addLine(to: CGPoint(x: 300, y: i))
            }
            context.stroke(
                path,
                with: .color(.gray.opacity(0.2)),
                lineWidth: 0.5
            )
            
            // Draw drawing path
            if drawingPath.count > 1 {
                var drawPath = Path()
                drawPath.move(to: drawingPath[0])
                
                for i in 1..<drawingPath.count {
                    drawPath.addLine(to: drawingPath[i])
                }
                
                context.stroke(
                    drawPath,
                    with: .color(.red),
                    lineWidth: 2
                )
            }
            
            // Draw current position
            if let lastPoint = drawingPath.last {
                var circle = Path()
                circle.addEllipse(in: CGRect(
                    x: lastPoint.x - 5,
                    y: lastPoint.y - 5,
                    width: 10,
                    height: 10
                ))
                context.fill(circle, with: .color(.red))
            }
            
            // Draw starting point
            if !drawingPath.isEmpty {
                var circle = Path()
                circle.addEllipse(in: CGRect(
                    x: drawingPath[0].x - 4,
                    y: drawingPath[0].y - 4,
                    width: 8,
                    height: 8
                ))
                context.fill(circle, with: .color(.green))
            }
        }
        .background(Color.white)
    }
}

#Preview {
    ContentView()
}
