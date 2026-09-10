#include "ShapeDrawer.h"

// =====================================================
// SHAPE DRAWER IMPLEMENTATION
// =====================================================

ShapeDrawer::ShapeDrawer(MotorControl* motors, ServoControl* servo) {
    _motors = motors;
    _servo = servo;
    _isDrawing = false;
    _drawingTimeMs = 0;
}

void ShapeDrawer::_calculateTiming(float sideLength, uint16_t& forwardMs, uint16_t& turnMs, float turnAngleDeg) {
    // Calculate forward duration
    forwardMs = (uint16_t)((sideLength / ROBOT_SPEED_MM_PER_S) * 1000.0f);
    
    // Calculate turn duration (1.8167 ms per degree)
    turnMs = (uint16_t)(turnAngleDeg * 1.8167f);
}

void ShapeDrawer::_drawSide(uint16_t forwardDurationMs) {
    _motors->setBothMotors(MOTOR_PWM_FORWARD, MOTOR_PWM_FORWARD);
    delay(forwardDurationMs);
    _motors->stop();
    delay(MOTOR_SETTLE_MS);
}

void ShapeDrawer::_turnRight(uint16_t turnDurationMs) {
    // Raise pen during turn
    _servo->penUp();
    delay(SERVO_TURN_SETTLE_MS);
    
    // Perform turn (differential drive)
    _motors->setBothMotors(MOTOR_PWM_TURN, -MOTOR_PWM_TURN);
    delay(turnDurationMs);
    _motors->stop();
    delay(MOTOR_SETTLE_MS);
    
    // Lower pen after turn
    _servo->penDown();
    delay(SERVO_TURN_SETTLE_MS);
}

void ShapeDrawer::drawSquare(float sideLength) {
    uint16_t forwardMs, turnMs;
    _calculateTiming(sideLength, forwardMs, turnMs, 90.0f);
    
    _isDrawing = true;
    _drawingTimeMs = 0;
    uint32_t startTime = millis();
    
    // Lower pen
    _servo->penDown();
    
    // Draw 4 sides
    for (uint8_t i = 0; i < 4; i++) {
        _drawSide(forwardMs);
        _turnRight(turnMs);
    }
    
    // Raise pen
    _servo->penUp();
    _motors->stop();
    
    _drawingTimeMs = millis() - startTime;
    _isDrawing = false;
}

void ShapeDrawer::drawRectangle(float length, float width) {
    // Rectangle: alternates between length and width sides
    uint16_t forwardMsLength, turnMs;
    uint16_t forwardMsWidth, dummyTurn;
    
    _calculateTiming(length, forwardMsLength, turnMs, 90.0f);
    _calculateTiming(width, forwardMsWidth, dummyTurn, 90.0f);
    
    _isDrawing = true;
    _drawingTimeMs = 0;
    uint32_t startTime = millis();
    
    _servo->penDown();
    
    // Draw rectangle: length, width, length, width
    _drawSide(forwardMsLength);
    _turnRight(turnMs);
    
    _drawSide(forwardMsWidth);
    _turnRight(turnMs);
    
    _drawSide(forwardMsLength);
    _turnRight(turnMs);
    
    _drawSide(forwardMsWidth);
    _turnRight(turnMs);
    
    _servo->penUp();
    _motors->stop();
    
    _drawingTimeMs = millis() - startTime;
    _isDrawing = false;
}

void ShapeDrawer::drawTriangle(float sideLength) {
    uint16_t forwardMs, turnMs;
    _calculateTiming(sideLength, forwardMs, turnMs, 120.0f);
    
    _isDrawing = true;
    _drawingTimeMs = 0;
    uint32_t startTime = millis();
    
    _servo->penDown();
    
    // Draw 3 sides (external angle 120°)
    for (uint8_t i = 0; i < 3; i++) {
        _drawSide(forwardMs);
        _turnRight(turnMs);
    }
    
    _servo->penUp();
    _motors->stop();
    
    _drawingTimeMs = millis() - startTime;
    _isDrawing = false;
}

void ShapeDrawer::drawPentagon(float sideLength) {
    uint16_t forwardMs, turnMs;
    _calculateTiming(sideLength, forwardMs, turnMs, 72.0f);
    
    _isDrawing = true;
    _drawingTimeMs = 0;
    uint32_t startTime = millis();
    
    _servo->penDown();
    
    // Draw 5 sides (external angle 72°)
    for (uint8_t i = 0; i < 5; i++) {
        _drawSide(forwardMs);
        _turnRight(turnMs);
    }
    
    _servo->penUp();
    _motors->stop();
    
    _drawingTimeMs = millis() - startTime;
    _isDrawing = false;
}

void ShapeDrawer::drawShape(uint8_t shapeType, uint8_t* params) {
    switch (shapeType) {
        case SHAPE_SQUARE:
            drawSquare(200.0f);
            break;
        
        case SHAPE_RECTANGLE: {
            float length = params[0] * 10.0f;  // Convert to mm
            float width = params[1] * 10.0f;
            drawRectangle(length, width);
            break;
        }
        
        case SHAPE_TRIANGLE:
            drawTriangle(200.0f);
            break;
        
        case SHAPE_PENTAGON:
            drawPentagon(200.0f);
            break;
        
        default:
            break;
    }
}

void ShapeDrawer::stop() {
    _motors->stop();
    _servo->penUp();
    _isDrawing = false;
}
