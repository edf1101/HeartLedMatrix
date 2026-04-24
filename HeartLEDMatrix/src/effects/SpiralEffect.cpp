#include "SpiralEffect.h"

SpiralEffect::SpiralEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper, 
                           uint8_t rotationSpeed, uint8_t spiralTightness, uint8_t numArms)
    : Effect(displayMatrix, ledMapper) 
{
    this->speed = rotationSpeed;
    this->tightness = spiralTightness;
    this->arms = numArms;
    this->timeBase = 0;
}

void SpiralEffect::setupEffect() {
    timeBase = 0;
    // Clear display
    for(uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
    matrix->show();
}
void SpiralEffect::loopEffect() {
    for (uint8_t i = 0; i < 79; i++) {
        LedMapper::Coordinate c = mapper->getCoord(i);
        if (c.x == 255) continue;

        int8_t dx = c.x - cx;
        int8_t dy = c.y - cy;

        uint8_t angle = atan2_8(dy, dx);
        uint8_t radius = dist_scaled(dx, dy);

        uint8_t phase = (angle * arms) + (radius * tightness) - (uint8_t)timeBase;

        // 1. Get the raw sine value
        uint16_t val = sin8(phase);

        // 2. Apply a Quadratic Curve
        // Multiplying the value by itself makes the "slopes" steeper.
        // This thins the ring and makes the edges darker/softer.
        uint8_t brightness = (val * val) >> 8;

        matrix->drawPixel(i, brightness);
    }

    matrix->show();
    timeBase += speed;
}