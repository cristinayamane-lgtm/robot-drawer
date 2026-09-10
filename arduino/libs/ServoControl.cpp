#include "ServoControl.h"

// =====================================================
// SERVO CONTROL IMPLEMENTATION
// =====================================================

ServoControl::ServoControl(uint8_t pin, uint8_t downAngle, uint8_t upAngle) {
    _pin = pin;
    _downAngle = downAngle;
    _upAngle = upAngle;
    _currentAngle = upAngle;  // Start with pen up
    _penDown = false;
    _settleMs = 1000;
}

void ServoControl::begin() {
    _servo.attach(_pin, 1000, 2000);
    delay(500);
    penUp();
    delay(500);
}

void ServoControl::penDown() {
    setPenAngle(_downAngle);
    _penDown = true;
    delay(_settleMs);
}

void ServoControl::penUp() {
    setPenAngle(_upAngle);
    _penDown = false;
    delay(_settleMs);
}

void ServoControl::setPenAngle(uint8_t angle) {
    angle = constrain(angle, 0, 180);
    _servo.write(angle);
    _currentAngle = angle;
    delay(100);  // Brief settle time
}
