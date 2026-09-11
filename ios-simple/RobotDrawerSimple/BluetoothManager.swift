import Foundation
import CoreBluetooth

// Bluetooth Protocol Constants
let START_BYTE: UInt8 = 0x7E
let END_BYTE: UInt8 = 0x7F

// Commands
let CMD_DRAW_SQUARE: UInt8 = 0x01
let CMD_DRAW_RECTANGLE: UInt8 = 0x02
let CMD_DRAW_TRIANGLE: UInt8 = 0x03
let CMD_DRAW_PENTAGON: UInt8 = 0x04
let CMD_STOP: UInt8 = 0x05
let CMD_PEN_UP: UInt8 = 0x06
let CMD_PEN_DOWN: UInt8 = 0x07
let CMD_REQUEST_STATUS: UInt8 = 0x08
let CMD_RESET: UInt8 = 0x09

// Responses
let RESP_STATUS_UPDATE: UInt8 = 0x10
let RESP_COMMAND_ACK: UInt8 = 0x11
let RESP_DRAWING_COMPLETE: UInt8 = 0x12
let RESP_ERROR: UInt8 = 0x13

// Frame Builder
class BluetoothFrameBuilder {
    static func buildFrame(command: UInt8, params: [UInt8] = []) -> Data {
        var frame: [UInt8] = [START_BYTE, command]
        frame.append(contentsOf: params)
        
        var checksum: UInt8 = command
        for param in params {
            checksum ^= param
        }
        
        frame.append(checksum)
        frame.append(END_BYTE)
        
        return Data(frame)
    }
}

// Bluetooth Manager
class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    @Published var isConnected = false
    @Published var statusMessage = "Disconnected"
    @Published var availableDevices: [CBPeripheral] = []
    @Published var isScanning = false
    @Published var connectionStatus = "Bluetooth ready"
    @Published var isDrawing = false
    
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
        statusMessage = "Looking for HC-06..."
        centralManager.scanForPeripherals(withServices: nil, options: nil)
    }
    
    func stopScanning() {
        isScanning = false
        centralManager.stopScan()
    }
    
    func connect(to peripheral: CBPeripheral) {
        connectionStatus = "Connecting..."
        statusMessage = "Connecting to HC-06..."
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
        statusMessage = "Failed to connect to HC-06"
        isConnected = false
    }
    
    func centralManager(
        _ central: CBCentralManager,
        didDisconnectPeripheral peripheral: CBPeripheral,
        error: Error?
    ) {
        connectionStatus = "Disconnected"
        statusMessage = "Disconnected from HC-06"
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
                statusMessage = "Connected to HC-06 ✅"
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
        
        let frameLen = rxBuffer.count - 1
        let receivedChecksum = rxBuffer[frameLen - 1]
        
        var calculatedChecksum: UInt8 = 0
        for i in 1..<(frameLen - 1) {
            calculatedChecksum ^= rxBuffer[i]
        }
        
        guard calculatedChecksum == receivedChecksum else {
            statusMessage = "Checksum error"
            return
        }
        
        let responseType = rxBuffer[1]
        
        switch responseType {
        case RESP_COMMAND_ACK:
            if rxBuffer.count >= 4 {
                let status = rxBuffer[3]
                let statusText = status == 0x00 ? "OK" : "ERROR"
                statusMessage = "Command received: \(statusText)"
            }
        
        case RESP_DRAWING_COMPLETE:
            DispatchQueue.main.async {
                self.isDrawing = false
                self.statusMessage = "Drawing complete! ✅"
            }
        
        case RESP_ERROR:
            if rxBuffer.count >= 3 {
                let errorCode = rxBuffer[2]
                statusMessage = "Error: 0x\(String(errorCode, radix: 16))"
            }
        
        default:
            break
        }
    }
    
    // MARK: - Send Commands
    
    func sendCommand(_ command: UInt8, params: [UInt8] = []) {
        guard isConnected, let characteristic = characteristicWrite else {
            statusMessage = "Not connected"
            return
        }
        
        let frame = BluetoothFrameBuilder.buildFrame(command: command, params: params)
        connectedPeripheral?.writeValue(frame, for: characteristic, type: .withResponse)
    }
    
    func drawSquare() {
        statusMessage = "Drawing square..."
        isDrawing = true
        sendCommand(CMD_DRAW_SQUARE)
    }
    
    func drawRectangle(length: UInt8, width: UInt8) {
        statusMessage = "Drawing rectangle..."
        isDrawing = true
        sendCommand(CMD_DRAW_RECTANGLE, params: [length, width])
    }
    
    func drawTriangle() {
        statusMessage = "Drawing triangle..."
        isDrawing = true
        sendCommand(CMD_DRAW_TRIANGLE)
    }
    
    func drawPentagon() {
        statusMessage = "Drawing pentagon..."
        isDrawing = true
        sendCommand(CMD_DRAW_PENTAGON)
    }
    
    func stop() {
        statusMessage = "Stopped"
        isDrawing = false
        sendCommand(CMD_STOP)
    }
    
    func reset() {
        statusMessage = "Resetting..."
        isDrawing = false
        sendCommand(CMD_RESET)
    }
}
