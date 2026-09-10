#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

// =====================================================
// SERVO CONTROL - PEN UP/DOWN
// =====================================================

class ServoControl {
public:
    // Constructor
    ServoControl(uint8_t pin, uint8_t downAngle = 95, uint8_t upAngle = 35);
    
    // Initialize servo
    void begin();
    
    // Pen control
    void penDown();
    void penUp();
    void setPenAngle(uint8_t angle);
    
    // State query
    uint8_t getPenAngle() { return _currentAngle; }
    bool isPenDown() { return _penDown; }
    
    // Timing
    void setSettleTime(uint16_t ms) { _settleMs = ms; }

private:
    Servo _servo;
    uint8_t _pin;
    uint8_t _downAngle;
    uint8_t _upAngle;
    uint8_t _currentAngle;
    bool _penDown;
    uint16_t _settleMs;
};

#endif // SERVO_CONTROL_H
