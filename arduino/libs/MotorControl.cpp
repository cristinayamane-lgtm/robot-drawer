#include "MotorControl.h"

// =====================================================
// MOTOR CONTROL IMPLEMENTATION
// =====================================================

MotorControl::MotorControl(
    uint8_t ain1, uint8_t ain2, uint8_t pwma,
    uint8_t bin1, uint8_t bin2, uint8_t pwmb,
    uint8_t stby
) {
    _pin_AIN1 = ain1;
    _pin_AIN2 = ain2;
    _pin_PWMA = pwma;
    _pin_BIN1 = bin1;
    _pin_BIN2 = bin2;
    _pin_PWMB = pwmb;
    _pin_STBY = stby;
    
    _speedA = 0;
    _speedB = 0;
    _enabled = false;
    
    // Motor direction correction
    _dirA = 1;   // Left motor
    _dirB = -1;  // Right motor (opposite direction for differential drive)
    
    _motorSettleMs = 1500;
}

void MotorControl::begin() {
    // Configure motor pins
    pinMode(_pin_AIN1, OUTPUT);
    pinMode(_pin_AIN2, OUTPUT);
    pinMode(_pin_PWMA, OUTPUT);
    pinMode(_pin_BIN1, OUTPUT);
    pinMode(_pin_BIN2, OUTPUT);
    pinMode(_pin_PWMB, OUTPUT);
    pinMode(_pin_STBY, OUTPUT);
    
    // Initialize to disabled state
    disable();
    stop();
}

void MotorControl::enable() {
    digitalWrite(_pin_STBY, HIGH);
    _enabled = true;
    delay(200);
}

void MotorControl::disable() {
    digitalWrite(_pin_STBY, LOW);
    _enabled = false;
}

void MotorControl::stop() {
    setMotorA(0);
    setMotorB(0);
}

void MotorControl::setMotorA(int16_t speed) {
    // Apply direction correction
    speed *= _dirA;
    speed = constrain(speed, -255, 255);
    
    _speedA = speed;
    _setMotorA(speed);
}

void MotorControl::setMotorB(int16_t speed) {
    // Apply direction correction
    speed *= _dirB;
    speed = constrain(speed, -255, 255);
    
    _speedB = speed;
    _setMotorB(speed);
}

void MotorControl::setBothMotors(int16_t speedA, int16_t speedB) {
    setMotorA(speedA);
    setMotorB(speedB);
}

void MotorControl::_setMotorA(int16_t speed) {
    if (speed > 0) {
        // Forward: AIN1=HIGH, AIN2=LOW
        digitalWrite(_pin_AIN1, HIGH);
        digitalWrite(_pin_AIN2, LOW);
        analogWrite(_pin_PWMA, speed);
    } else if (speed < 0) {
        // Backward: AIN1=LOW, AIN2=HIGH
        digitalWrite(_pin_AIN1, LOW);
        digitalWrite(_pin_AIN2, HIGH);
        analogWrite(_pin_PWMA, -speed);
    } else {
        // Stop: AIN1=LOW, AIN2=LOW
        digitalWrite(_pin_AIN1, LOW);
        digitalWrite(_pin_AIN2, LOW);
        analogWrite(_pin_PWMA, 0);
    }
}

void MotorControl::_setMotorB(int16_t speed) {
    if (speed > 0) {
        // Forward: BIN1=HIGH, BIN2=LOW
        digitalWrite(_pin_BIN1, HIGH);
        digitalWrite(_pin_BIN2, LOW);
        analogWrite(_pin_PWMB, speed);
    } else if (speed < 0) {
        // Backward: BIN1=LOW, BIN2=HIGH
        digitalWrite(_pin_BIN1, LOW);
        digitalWrite(_pin_BIN2, HIGH);
        analogWrite(_pin_PWMB, -speed);
    } else {
        // Stop: BIN1=LOW, BIN2=LOW
        digitalWrite(_pin_BIN1, LOW);
        digitalWrite(_pin_BIN2, LOW);
        analogWrite(_pin_PWMB, 0);
    }
}

void MotorControl::driveForward(uint16_t durationMs, uint8_t pwm) {
    setBothMotors(pwm, pwm);
    delay(durationMs);
    stop();
    delay(_motorSettleMs);
}

void MotorControl::turnRight(uint16_t durationMs, uint8_t pwm) {
    setBothMotors(pwm, -pwm);
    delay(durationMs);
    stop();
    delay(_motorSettleMs);
}

void MotorControl::turnLeft(uint16_t durationMs, uint8_t pwm) {
    setBothMotors(-pwm, pwm);
    delay(durationMs);
    stop();
    delay(_motorSettleMs);
}
