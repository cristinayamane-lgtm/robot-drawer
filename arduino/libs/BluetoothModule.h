#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include <Arduino.h>
#include <stdint.h>

// =====================================================
// BLUETOOTH PROTOCOL - FRAME STRUCTURE
// =====================================================

#define START_BYTE 0x7E
#define END_BYTE 0x7F

// Command Types (iPhone -> Arduino)
#define CMD_DRAW_SQUARE 0x01
#define CMD_DRAW_RECTANGLE 0x02
#define CMD_DRAW_TRIANGLE 0x03
#define CMD_DRAW_PENTAGON 0x04
#define CMD_STOP 0x05
#define CMD_PEN_UP 0x06
#define CMD_PEN_DOWN 0x07
#define CMD_REQUEST_STATUS 0x08
#define CMD_RESET 0x09

// Response Types (Arduino -> iPhone)
#define RESP_STATUS_UPDATE 0x10
#define RESP_COMMAND_ACK 0x11
#define RESP_DRAWING_COMPLETE 0x12
#define RESP_ERROR 0x13

// Error Codes
#define ERROR_INVALID_CHECKSUM 0x01
#define ERROR_UNKNOWN_COMMAND 0x02
#define ERROR_INVALID_PARAMS 0x03
#define ERROR_DRAWING_IN_PROGRESS 0x04
#define ERROR_MOTOR_ERROR 0x05
#define ERROR_SERVO_ERROR 0x06

// =====================================================
// BLUETOOTH MODULE CLASS
// =====================================================

class BluetoothModule {
public:
    // Constructor
    BluetoothModule(uint16_t baudRate = 9600);
    
    // Initialize Bluetooth communication
    void begin();
    
    // Check if data is available
    bool available();
    
    // Parse incoming command from iPhone
    bool parseCommand(uint8_t& cmdType, uint8_t* params, uint8_t& paramLen);
    
    // Send status update (position + pen state)
    void sendStatusUpdate(uint16_t x, uint16_t y, uint8_t penState);
    
    // Send command acknowledgment
    void sendCommandAck(uint8_t cmdType, uint8_t status);
    
    // Send drawing complete notification
    void sendDrawingComplete(uint16_t durationSeconds);
    
    // Send error response
    void sendError(uint8_t errorCode);
    
    // Debug print (via Serial)
    void debugPrint(const char* message);

private:
    uint16_t _baudRate;
    uint8_t _rxBuffer[32];
    uint8_t _rxIndex;
    bool _frameStarted;
    
    // Helper functions
    uint8_t calculateChecksum(uint8_t* data, uint8_t length);
    void sendFrame(uint8_t responseType, uint8_t* data, uint8_t dataLen);
};

#endif // BLUETOOTH_MODULE_H
