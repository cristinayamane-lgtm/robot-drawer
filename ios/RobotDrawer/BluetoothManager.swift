import Foundation
import CoreBluetooth

// =====================================================
// BLUETOOTH PROTOCOL DEFINITIONS
// =====================================================

let START_BYTE: UInt8 = 0x7E
let END_BYTE: UInt8 = 0x7F

// Commands (iPhone -> Arduino)
let CMD_DRAW_SQUARE: UInt8 = 0x01
let CMD_DRAW_RECTANGLE: UInt8 = 0x02
let CMD_DRAW_TRIANGLE: UInt8 = 0x03
let CMD_DRAW_PENTAGON: UInt8 = 0x04
let CMD_STOP: UInt8 = 0x05
let CMD_PEN_UP: UInt8 = 0x06
let CMD_PEN_DOWN: UInt8 = 0x07
let CMD_REQUEST_STATUS: UInt8 = 0x08
let CMD_RESET: UInt8 = 0x09

// Responses (Arduino -> iPhone)
let RESP_STATUS_UPDATE: UInt8 = 0x10
let RESP_COMMAND_ACK: UInt8 = 0x11
let RESP_DRAWING_COMPLETE: UInt8 = 0x12
let RESP_ERROR: UInt8 = 0x13

// Error Codes
let ERROR_INVALID_CHECKSUM: UInt8 = 0x01
let ERROR_UNKNOWN_COMMAND: UInt8 = 0x02
let ERROR_INVALID_PARAMS: UInt8 = 0x03
let ERROR_DRAWING_IN_PROGRESS: UInt8 = 0x04
let ERROR_MOTOR_ERROR: UInt8 = 0x05
let ERROR_SERVO_ERROR: UInt8 = 0x06

// =====================================================
// BLUETOOTH FRAME BUILDER
// =====================================================

class BluetoothFrameBuilder {
    static func buildFrame(command: UInt8, params: [UInt8] = []) -> Data {
        var frame: [UInt8] = [START_BYTE, command]
        frame.append(contentsOf: params)
        
        // Calculate checksum (XOR of all bytes except START and END)
        var checksum: UInt8 = command
        for param in params {
            checksum ^= param
        }
        
        frame.append(checksum)
        frame.append(END_BYTE)
        
        return Data(frame)
    }
}

// =====================================================
// ROBOT STATUS
// =====================================================

struct RobotStatus {
    var x: UInt16 = 0
    var y: UInt16 = 0
    var penDown: Bool = false
    var isDrawing: Bool = false
    var lastMessage: String = "Ready"
    var drawingTime: UInt16 = 0
}

// =====================================================
// BLUETOOTH MANAGER - MANAGES CONNECTION & COMMUNICATION
// =====================================================

class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    @Published var isConnected = false
    @Published var robotStatus = RobotStatus()
    @Published var availableDevices: [CBPeripheral] = []
    @Published var drawingPath: [CGPoint] = []
    @Published var connectionStatus = "Disconnected"
    @Published var isScanning = false
    
    private var centralManager: CBCentralManager!
    private var connectedPeripheral: CBPeripheral?
    private var characteristicWrite: CBCharacteristic?
    private var rxBuffer: [UInt8] = []
    private var frameStarted = false
    
    private let HC06_SERVICE_UUID = CBUUID(string: "FFE0")
    private let HC06_CHARACTERISTIC_UUID = CBUUID(string: "FFE1")
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: .main)
    }
    
    // MARK: - Scanning
    
    func startScanning() {
        isScanning = true
        availableDevices = []
        connectionStatus = "Scanning..."
        centralManager.scanForPeripherals(withServices: nil, options: nil)
    }
    
    func stopScanning() {
        isScanning = false
        centralManager.stopScan()
    }
    
    func connect(to peripheral: CBPeripheral) {
        connectionStatus = "Connecting..."
        centralManager.connect(peripheral, options: nil)
    }
    
    func disconnect() {
        if let peripheral = connectedPeripheral {
            centralManager.cancelPeripheralConnection(peripheral)
        }
    }
    
    // MARK: - CBCentralManagerDelegate
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            connectionStatus = "Bluetooth ready"
        case .poweredOff:
            connectionStatus = "Bluetooth off"
            isConnected = false
        case .resetting:
            connectionStatus = "Resetting..."
        case .unauthorized:
            connectionStatus = "Unauthorized"
        case .unsupported:
            connectionStatus = "Not supported"
        case .unknown:
            connectionStatus = "Unknown state"
        @unknown default:
            connectionStatus = "Unknown"
        }
    }
    
    func centralManager(
        _ central: CBCentralManager,
        didDiscover peripheral: CBPeripheral,
        advertisementData: [String : Any],
        rssi RSSI: NSNumber
    ) {
        if peripheral.name?.contains("HC-06") ?? false || peripheral.name?.contains("HC06") ?? false {
            if !availableDevices.contains(peripheral) {
                availableDevices.append(peripheral)
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        connectedPeripheral = peripheral
        peripheral.delegate = self
        peripheral.discoverServices([HC06_SERVICE_UUID])
        connectionStatus = "Discovering services..."
    }
    
    func centralManager(
        _ central: CBCentralManager,
        didFailToConnect peripheral: CBPeripheral,
        error: Error?
    ) {
        connectionStatus = "Connection failed"
        isConnected = false
    }
    
    func centralManager(
        _ central: CBCentralManager,
        didDisconnectPeripheral peripheral: CBPeripheral,
        error: Error?
    ) {
        connectionStatus = "Disconnected"
        isConnected = false
        connectedPeripheral = nil
        characteristicWrite = nil
    }
    
    // MARK: - CBPeripheralDelegate
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        
        for service in services {
            peripheral.discoverCharacteristics([HC06_CHARACTERISTIC_UUID], for: service)
        }
    }
    
    func peripheral(
        _ peripheral: CBPeripheral,
        didDiscoverCharacteristicsFor service: CBService,
        error: Error?
    ) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            if characteristic.uuid == HC06_CHARACTERISTIC_UUID {
                characteristicWrite = characteristic
                peripheral.setNotifyValue(true, for: characteristic)
                isConnected = true
                connectionStatus = "Connected"
                robotStatus.lastMessage = "Connected to HC-06"
            }
        }
    }
    
    func peripheral(
        _ peripheral: CBPeripheral,
        didUpdateValueFor characteristic: CBCharacteristic,
        error: Error?
    ) {
        guard let data = characteristic.value else { return }
        
        parseIncomingData(data)
    }
    
    // MARK: - Data Parsing
    
    private func parseIncomingData(_ data: Data) {
        for byte in data {
            if !frameStarted && byte == START_BYTE {
                frameStarted = true
                rxBuffer = [byte]
                continue
            }
            
            if frameStarted {
                rxBuffer.append(byte)
                
                if byte == END_BYTE && rxBuffer.count >= 5 {
                    processFrame()
                    frameStarted = false
                    rxBuffer = []
                }
            }
        }
    }
    
    private func processFrame() {
        guard rxBuffer.count >= 5 else { return }
        
        let frameLen = rxBuffer.count - 1 // Excluding END_BYTE
        let receivedChecksum = rxBuffer[frameLen - 1]
        
        // Verify checksum
        var calculatedChecksum: UInt8 = 0
        for i in 1..<(frameLen - 1) {
            calculatedChecksum ^= rxBuffer[i]
        }
        
        guard calculatedChecksum == receivedChecksum else {
            robotStatus.lastMessage = "Checksum error"
            return
        }
        
        let responseType = rxBuffer[1]
        
        switch responseType {
        case RESP_STATUS_UPDATE:
            if rxBuffer.count >= 7 {
                let xHi = UInt16(rxBuffer[2])
                let xLo = UInt16(rxBuffer[3])
                let yHi = UInt16(rxBuffer[4])
                let yLo = UInt16(rxBuffer[5])
                let penState = rxBuffer[6]
                
                let x = (xHi << 8) | xLo
                let y = (yHi << 8) | yLo
                
                DispatchQueue.main.async {
                    self.robotStatus.x = x
                    self.robotStatus.y = y
                    self.robotStatus.penDown = penState == 0x01
                    self.robotStatus.isDrawing = true
                    
                    // Add to drawing path
                    let point = CGPoint(
                        x: CGFloat(x) / 297.0 * 300,  // Scale to screen
                        y: CGFloat(y) / 420.0 * 400
                    )
                    self.drawingPath.append(point)
                }
            }
        
        case RESP_COMMAND_ACK:
            if rxBuffer.count >= 4 {
                let cmd = rxBuffer[2]
                let status = rxBuffer[3]
                let statusText = status == 0x00 ? "OK" : "ERROR"
                robotStatus.lastMessage = "Command 0x\(String(cmd, radix: 16)) \(statusText)"
            }
        
        case RESP_DRAWING_COMPLETE:
            if rxBuffer.count >= 5 {
                let timeHi = UInt16(rxBuffer[2])
                let timeLo = UInt16(rxBuffer[3])
                let time = (timeHi << 8) | timeLo
                
                DispatchQueue.main.async {
                    self.robotStatus.drawingTime = time
                    self.robotStatus.isDrawing = false
                    self.robotStatus.lastMessage = "Drawing complete! Time: \(time)s"
                }
            }
        
        case RESP_ERROR:
            if rxBuffer.count >= 3 {
                let errorCode = rxBuffer[2]
                let errorMsg: String
                switch errorCode {
                case ERROR_INVALID_CHECKSUM:
                    errorMsg = "Invalid checksum"
                case ERROR_UNKNOWN_COMMAND:
                    errorMsg = "Unknown command"
                case ERROR_INVALID_PARAMS:
                    errorMsg = "Invalid parameters"
                case ERROR_DRAWING_IN_PROGRESS:
                    errorMsg = "Drawing in progress"
                case ERROR_MOTOR_ERROR:
                    errorMsg = "Motor error"
                case ERROR_SERVO_ERROR:
                    errorMsg = "Servo error"
                default:
                    errorMsg = "Unknown error 0x\(String(errorCode, radix: 16))"
                }
                robotStatus.lastMessage = "Error: \(errorMsg)"
            }
        
        default:
            robotStatus.lastMessage = "Unknown response 0x\(String(responseType, radix: 16))"
        }
    }
    
    // MARK: - Send Commands
    
    func sendCommand(_ command: UInt8, params: [UInt8] = []) {
        guard isConnected, let characteristic = characteristicWrite else {
            robotStatus.lastMessage = "Not connected"
            return
        }
        
        let frame = BluetoothFrameBuilder.buildFrame(command: command, params: params)
        connectedPeripheral?.writeValue(frame, for: characteristic, type: .withResponse)
    }
    
    func drawSquare() {
        drawingPath = []
        robotStatus.lastMessage = "Drawing square..."
        sendCommand(CMD_DRAW_SQUARE)
    }
    
    func drawRectangle(length: UInt8, width: UInt8) {
        drawingPath = []
        robotStatus.lastMessage = "Drawing rectangle..."
        sendCommand(CMD_DRAW_RECTANGLE, params: [length, width])
    }
    
    func drawTriangle() {
        drawingPath = []
        robotStatus.lastMessage = "Drawing triangle..."
        sendCommand(CMD_DRAW_TRIANGLE)
    }
    
    func drawPentagon() {
        drawingPath = []
        robotStatus.lastMessage = "Drawing pentagon..."
        sendCommand(CMD_DRAW_PENTAGON)
    }
    
    func stop() {
        robotStatus.lastMessage = "Stopped"
        sendCommand(CMD_STOP)
    }
    
    func penUp() {
        sendCommand(CMD_PEN_UP)
    }
    
    func penDown() {
        sendCommand(CMD_PEN_DOWN)
    }
    
    func requestStatus() {
        sendCommand(CMD_REQUEST_STATUS)
    }
    
    func reset() {
        drawingPath = []
        robotStatus.lastMessage = "Resetting..."
        sendCommand(CMD_RESET)
    }
}
