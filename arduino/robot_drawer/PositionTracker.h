#ifndef POSITION_TRACKER_H
#define POSITION_TRACKER_H

#include <Arduino.h>
#include <stdint.h>

// =====================================================
// POSITION TRACKER - MAINTAINS X,Y COORDINATES
// =====================================================

class PositionTracker {
public:
    // Constructor
    PositionTracker();
    
    // Position control
    void reset();
    void setPosition(uint16_t x, uint16_t y);
    void moveFoward(uint16_t distanceMm, float robotSpeed = 654.0f);
    void rotate(float angleDeg);
    
    // State query
    uint16_t getX() { return _x; }
    uint16_t getY() { return _y; }
    float getHeading() { return _heading; }
    
    // Canvas size (A3 sheet: 297 x 420 mm)
    void setCanvasSize(uint16_t width, uint16_t height) {
        _canvasWidth = width;
        _canvasHeight = height;
    }

private:
    uint16_t _x, _y;           // Current position
    float _heading;             // Current direction (degrees, 0° = right, 90° = up)
    uint16_t _canvasWidth;      // Canvas width (mm)
    uint16_t _canvasHeight;     // Canvas height (mm)
};

#endif // POSITION_TRACKER_H
