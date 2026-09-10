#include "BluetoothModule.h"

// =====================================================
// BLUETOOTH MODULE IMPLEMENTATION
// =====================================================

BluetoothModule::BluetoothModule(uint16_t baudRate) {
    _baudRate = baudRate;
    _rxIndex = 0;
    _frameStarted = false;
}

void BluetoothModule::begin() {
    Serial.begin(_baudRate);
    debugPrint("[BT] Bluetooth module initialized at ");
    debugPrint(String(_baudRate).c_str());
    debugPrint(" baud\n");
}

bool BluetoothModule::available() {
    return Serial.available() > 0;
}

bool BluetoothModule::parseCommand(uint8_t& cmdType, uint8_t* params, uint8_t& paramLen) {
    while (Serial.available()) {
        uint8_t byte = Serial.read();
        
        // Wait for frame start
        if (!_frameStarted) {
            if (byte == START_BYTE) {
                _frameStarted = true;
                _rxIndex = 0;
                _rxBuffer[_rxIndex++] = byte;
            }
            continue;
        }
        
        // Collect frame data
        _rxBuffer[_rxIndex++] = byte;
        
        // Check for frame end
        if (byte == END_BYTE && _rxIndex >= 5) {
            // Frame format: [START] [CMD] [PARAM1] ... [CHECKSUM] [END]
            // Minimum: START(1) + CMD(1) + CHECKSUM(1) + END(1) = 4 bytes total, _rxIndex >= 5
            
            uint8_t frameLen = _rxIndex - 1; // Excluding END_BYTE
            uint8_t receivedChecksum = _rxBuffer[frameLen - 1];
            
            // Calculate checksum (all bytes except START and END)
            uint8_t calculatedChecksum = 0;
            for (uint8_t i = 1; i < frameLen - 1; i++) {
                calculatedChecksum ^= _rxBuffer[i];
            }
            
            // Verify checksum
            if (calculatedChecksum != receivedChecksum) {
                debugPrint("[BT] ERROR: Invalid checksum\n");
                sendError(ERROR_INVALID_CHECKSUM);
                _frameStarted = false;
                return false;
            }
            
            // Extract command type
            cmdType = _rxBuffer[1];
            
            // Extract parameters
            paramLen = frameLen - 3; // CMD + CHECKSUM + END
            if (paramLen > 0) {
                for (uint8_t i = 0; i < paramLen; i++) {
                    params[i] = _rxBuffer[2 + i];
                }
            }
            
            debugPrint("[BT] Command received: 0x");
            debugPrint(String(cmdType, HEX).c_str());
            debugPrint(" with ");
            debugPrint(String(paramLen).c_str());
            debugPrint(" params\n");
            
            _frameStarted = false;
            return true;
        }
        
        // Prevent buffer overflow
        if (_rxIndex >= sizeof(_rxBuffer)) {
            debugPrint("[BT] ERROR: RX buffer overflow\n");
            _frameStarted = false;
            return false;
        }
    }
    
    return false;
}

void BluetoothModule::sendStatusUpdate(uint16_t x, uint16_t y, uint8_t penState) {
    uint8_t data[5];
    data[0] = (x >> 8) & 0xFF;     // X HIGH
    data[1] = x & 0xFF;             // X LOW
    data[2] = (y >> 8) & 0xFF;     // Y HIGH
    data[3] = y & 0xFF;             // Y LOW
    data[4] = penState;             // PEN STATE
    
    sendFrame(RESP_STATUS_UPDATE, data, 5);
}

void BluetoothModule::sendCommandAck(uint8_t cmdType, uint8_t status) {
    uint8_t data[2];
    data[0] = cmdType;  // Echo command
    data[1] = status;   // 0x00 = OK, 0x01 = ERROR
    
    sendFrame(RESP_COMMAND_ACK, data, 2);
}

void BluetoothModule::sendDrawingComplete(uint16_t durationSeconds) {
    uint8_t data[2];
    data[0] = (durationSeconds >> 8) & 0xFF;  // TIME HIGH
    data[1] = durationSeconds & 0xFF;         // TIME LOW
    
    sendFrame(RESP_DRAWING_COMPLETE, data, 2);
}

void BluetoothModule::sendError(uint8_t errorCode) {
    uint8_t data[1];
    data[0] = errorCode;
    
    sendFrame(RESP_ERROR, data, 1);
}

void BluetoothModule::sendFrame(uint8_t responseType, uint8_t* data, uint8_t dataLen) {
    // Calculate total frame size
    uint8_t frameSize = 4 + dataLen; // START + TYPE + DATA + CHECKSUM + END
    uint8_t frame[frameSize];
    
    // Build frame
    frame[0] = START_BYTE;
    frame[1] = responseType;
    
    // Copy data
    for (uint8_t i = 0; i < dataLen; i++) {
        frame[2 + i] = data[i];
    }
    
    // Calculate checksum (TYPE + DATA + CHECKSUM itself)
    uint8_t checksum = 0;
    for (uint8_t i = 1; i < frameSize - 2; i++) {
        checksum ^= frame[i];
    }
    frame[frameSize - 2] = checksum;
    frame[frameSize - 1] = END_BYTE;
    
    // Send frame
    for (uint8_t i = 0; i < frameSize; i++) {
        Serial.write(frame[i]);
    }
}

uint8_t BluetoothModule::calculateChecksum(uint8_t* data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void BluetoothModule::debugPrint(const char* message) {
    // Uncomment for debug output
    // Serial.print(message);
}
