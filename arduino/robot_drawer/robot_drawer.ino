#include <Arduino.h>
#include <Servo.h>
#include "../libs/BluetoothModule.h"
#include "../libs/MotorControl.h"
#include "../libs/ServoControl.h"
#include "ShapeDrawer.h"
#include "PositionTracker.h"

// =====================================================
// ROBOT DRAWER - MAIN PROGRAM
// =====================================================
// Draws geometric shapes via Bluetooth commands from iPhone
// Sends position feedback in real-time via Bluetooth
// =====================================================

// =====================================================
// PIN DEFINITIONS - TB6612FNG MOTOR DRIVER
// =====================================================

const uint8_t PIN_AIN1 = 2;
const uint8_t PIN_AIN2 = 4;
const uint8_t PIN_PWMA = 5;

const uint8_t PIN_BIN1 = 7;
const uint8_t PIN_BIN2 = 8;
const uint8_t PIN_PWMB = 6;

const uint8_t PIN_STBY = 12;
const uint8_t PIN_SERVO = 9;

// =====================================================
// GLOBAL OBJECTS
// =====================================================

BluetoothModule bluetooth(9600);
MotorControl motors(
    PIN_AIN1, PIN_AIN2, PIN_PWMA,
    PIN_BIN1, PIN_BIN2, PIN_PWMB,
    PIN_STBY
);
ServoControl servo(PIN_SERVO, 95, 35);  // down=95°, up=35°
ShapeDrawer shapeDrawer(&motors, &servo);
PositionTracker positionTracker;

// =====================================================
// STATE VARIABLES
// =====================================================

bool isDrawing = false;
uint32_t lastStatusUpdateTime = 0;
const uint16_t STATUS_UPDATE_INTERVAL = 100;  // Send status every 100ms

// =====================================================
// BLUETOOTH COMMAND HANDLER
// =====================================================

void handleBluetoothCommand(uint8_t cmdType, uint8_t* params, uint8_t paramLen) {
    if (isDrawing) {
        bluetooth.sendError(ERROR_DRAWING_IN_PROGRESS);
        return;
    }
    
    switch (cmdType) {
        case CMD_DRAW_SQUARE:
            Serial.println("[CMD] Draw Square");
            bluetooth.sendCommandAck(cmdType, 0x00);  // OK
            isDrawing = true;
            shapeDrawer.drawSquare(200.0f);
            isDrawing = false;
            bluetooth.sendDrawingComplete(shapeDrawer.getDrawingTime());
            positionTracker.reset();  // Reset position after drawing
            break;
        
        case CMD_DRAW_RECTANGLE:
            if (paramLen >= 2) {
                float length = (float)params[0] * 10.0f;  // Convert to mm
                float width = (float)params[1] * 10.0f;
                Serial.print("[CMD] Draw Rectangle: ");
                Serial.print(length);
                Serial.print("mm x ");
                Serial.println(width);
                bluetooth.sendCommandAck(cmdType, 0x00);
                isDrawing = true;
                shapeDrawer.drawRectangle(length, width);
                isDrawing = false;
                bluetooth.sendDrawingComplete(shapeDrawer.getDrawingTime());
                positionTracker.reset();
            } else {
                bluetooth.sendError(ERROR_INVALID_PARAMS);
            }
            break;
        
        case CMD_DRAW_TRIANGLE:
            Serial.println("[CMD] Draw Triangle");
            bluetooth.sendCommandAck(cmdType, 0x00);
            isDrawing = true;
            shapeDrawer.drawTriangle(200.0f);
            isDrawing = false;
            bluetooth.sendDrawingComplete(shapeDrawer.getDrawingTime());
            positionTracker.reset();
            break;
        
        case CMD_DRAW_PENTAGON:
            Serial.println("[CMD] Draw Pentagon");
            bluetooth.sendCommandAck(cmdType, 0x00);
            isDrawing = true;
            shapeDrawer.drawPentagon(200.0f);
            isDrawing = false;
            bluetooth.sendDrawingComplete(shapeDrawer.getDrawingTime());
            positionTracker.reset();
            break;
        
        case CMD_STOP:
            Serial.println("[CMD] Stop");
            shapeDrawer.stop();
            isDrawing = false;
            bluetooth.sendCommandAck(cmdType, 0x00);
            break;
        
        case CMD_PEN_UP:
            Serial.println("[CMD] Pen Up");
            servo.penUp();
            bluetooth.sendCommandAck(cmdType, 0x00);
            break;
        
        case CMD_PEN_DOWN:
            Serial.println("[CMD] Pen Down");
            servo.penDown();
            bluetooth.sendCommandAck(cmdType, 0x00);
            break;
        
        case CMD_REQUEST_STATUS:
            Serial.println("[CMD] Request Status");
            bluetooth.sendStatusUpdate(
                positionTracker.getX(),
                positionTracker.getY(),
                servo.isPenDown() ? 0x01 : 0x00
            );
            break;
        
        case CMD_RESET:
            Serial.println("[CMD] Reset");
            shapeDrawer.stop();
            positionTracker.reset();
            isDrawing = false;
            bluetooth.sendCommandAck(cmdType, 0x00);
            break;
        
        default:
            Serial.println("[CMD] Unknown command");
            bluetooth.sendError(ERROR_UNKNOWN_COMMAND);
            break;
    }
}

// =====================================================
// PERIODIC STATUS UPDATES
// =====================================================

void sendPeriodicStatusUpdate() {
    uint32_t currentTime = millis();
    
    if (isDrawing && (currentTime - lastStatusUpdateTime) >= STATUS_UPDATE_INTERVAL) {
        bluetooth.sendStatusUpdate(
            positionTracker.getX(),
            positionTracker.getY(),
            servo.isPenDown() ? 0x01 : 0x00
        );
        lastStatusUpdateTime = currentTime;
    }
}

// =====================================================
// INITIALIZATION
// =====================================================

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    delay(100);
    Serial.println("\n\n=== ROBOT DRAWER INITIALIZING ===");
    
    // Initialize Bluetooth
    bluetooth.begin();
    
    // Initialize motors
    motors.begin();
    delay(100);
    
    // Initialize servo
    servo.begin();
    delay(100);
    
    // Enable motors
    motors.enable();
    
    // Initialize position tracker
    positionTracker.setCanvasSize(297, 420);  // A3 sheet size
    
    Serial.println("=== ROBOT READY ===");
    Serial.println("Waiting for Bluetooth commands...");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
    // Check for Bluetooth commands
    if (bluetooth.available()) {
        uint8_t cmdType;
        uint8_t params[4];
        uint8_t paramLen = 0;
        
        if (bluetooth.parseCommand(cmdType, params, paramLen)) {
            handleBluetoothCommand(cmdType, params, paramLen);
        }
    }
    
    // Send periodic status updates during drawing
    if (isDrawing) {
        sendPeriodicStatusUpdate();
    }
    
    delay(10);  // Small delay to prevent overwhelming the loop
}
