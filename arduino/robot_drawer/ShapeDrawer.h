#ifndef SHAPE_DRAWER_H
#define SHAPE_DRAWER_H

#include <Arduino.h>
#include "../libs/MotorControl.h"
#include "../libs/ServoControl.h"
#include <stdint.h>

// =====================================================
// SHAPE DEFINITIONS
// =====================================================

#define SHAPE_NONE 0
#define SHAPE_SQUARE 1
#define SHAPE_RECTANGLE 2
#define SHAPE_TRIANGLE 3
#define SHAPE_PENTAGON 4

// Calibrated values
#define ROBOT_SPEED_MM_PER_S 654.0f
#define MOTOR_PWM_FORWARD 180
#define MOTOR_PWM_TURN 170
#define SERVO_TURN_SETTLE_MS 500
#define SERVO_SETTLE_MS 1000
#define MOTOR_SETTLE_MS 1500

// =====================================================
// SHAPE DRAWER CLASS
// =====================================================

class ShapeDrawer {
public:
    // Constructor
    ShapeDrawer(MotorControl* motors, ServoControl* servo);
    
    // Drawing functions
    void drawSquare(float sideLength = 200.0f);
    void drawRectangle(float length, float width);
    void drawTriangle(float sideLength = 200.0f);
    void drawPentagon(float sideLength = 200.0f);
    
    // Utility functions
    void drawShape(uint8_t shapeType, uint8_t* params);
    void stop();
    
    // State query
    bool isDrawing() { return _isDrawing; }
    uint16_t getDrawingTime() { return _drawingTimeMs / 1000; }

private:
    MotorControl* _motors;
    ServoControl* _servo;
    bool _isDrawing;
    uint32_t _drawingTimeMs;
    
    // Helper functions
    void _drawSide(uint16_t forwardDurationMs);
    void _turnRight(uint16_t turnDurationMs);
    void _calculateTiming(float sideLength, uint16_t& forwardMs, uint16_t& turnMs, float turnAngleDeg);
};

#endif // SHAPE_DRAWER_H
