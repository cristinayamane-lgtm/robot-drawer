#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <stdint.h>

// =====================================================
// MOTOR CONTROL - TB6612FNG DRIVER
// =====================================================

class MotorControl {
public:
    // Constructor
    MotorControl(
        uint8_t ain1, uint8_t ain2, uint8_t pwma,
        uint8_t bin1, uint8_t bin2, uint8_t pwmb,
        uint8_t stby
    );
    
    // Initialize motor control
    void begin();
    
    // Set motor speeds
    void setMotorA(int16_t speed);
    void setMotorB(int16_t speed);
    void setBothMotors(int16_t speedA, int16_t speedB);
    
    // Stop motors
    void stop();
    
    // Enable/Disable standby
    void enable();
    void disable();
    
    // Movement functions
    void driveForward(uint16_t durationMs, uint8_t pwm);
    void turnRight(uint16_t durationMs, uint8_t pwm);
    void turnLeft(uint16_t durationMs, uint8_t pwm);
    
    // Get current motor state
    int16_t getMotorASpeed() { return _speedA; }
    int16_t getMotorBSpeed() { return _speedB; }
    bool isEnabled() { return _enabled; }

private:
    // TB6612 pins
    uint8_t _pin_AIN1, _pin_AIN2, _pin_PWMA;
    uint8_t _pin_BIN1, _pin_BIN2, _pin_PWMB;
    uint8_t _pin_STBY;
    
    // Motor state
    int16_t _speedA, _speedB;
    bool _enabled;
    
    // Motor direction correction
    int8_t _dirA, _dirB;
    
    // Timing
    uint16_t _motorSettleMs;
    
    // Helper functions
    void _setMotorA(int16_t speed);
    void _setMotorB(int16_t speed);
};

#endif // MOTOR_CONTROL_H
