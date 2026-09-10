#include "PositionTracker.h"
#include <math.h>

// =====================================================
// POSITION TRACKER IMPLEMENTATION
// =====================================================

PositionTracker::PositionTracker() {
    _x = 0;
    _y = 0;
    _heading = 0.0f;  // Facing right
    _canvasWidth = 297;  // A3 width (mm)
    _canvasHeight = 420; // A3 height (mm)
}

void PositionTracker::reset() {
    _x = 0;
    _y = 0;
    _heading = 0.0f;
}

void PositionTracker::setPosition(uint16_t x, uint16_t y) {
    _x = constrain(x, 0, _canvasWidth);
    _y = constrain(y, 0, _canvasHeight);
}

void PositionTracker::moveFoward(uint16_t distanceMm, float robotSpeed) {
    // Calculate new position based on heading
    float radians = (_heading * PI) / 180.0f;
    
    int16_t deltaX = (int16_t)(distanceMm * cos(radians));
    int16_t deltaY = (int16_t)(distanceMm * sin(radians));
    
    _x = constrain((int16_t)_x + deltaX, 0, _canvasWidth);
    _y = constrain((int16_t)_y + deltaY, 0, _canvasHeight);
}

void PositionTracker::rotate(float angleDeg) {
    _heading += angleDeg;
    
    // Normalize heading to 0-360
    while (_heading >= 360.0f) _heading -= 360.0f;
    while (_heading < 0.0f) _heading += 360.0f;
}
