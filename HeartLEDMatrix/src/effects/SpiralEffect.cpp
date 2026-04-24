/**
 * @file SpiralEffect.cpp
 * @brief Implementation of the rotating spiral animation effect.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "SpiralEffect.h"

SpiralEffect::SpiralEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                           uint8_t rotationSpeed, uint8_t spiralTightness, uint8_t numArms)
    : Effect(displayMatrix, ledMapper), speed(rotationSpeed), tightness(spiralTightness),
      arms(numArms), timeBase(0) {}

void SpiralEffect::setupEffect() {
    timeBase = 0;
    for (uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
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

        // Combine angular and radial components to create spiral
        uint8_t phase = (angle * arms) + (radius * tightness) - (uint8_t)timeBase;

        // Apply sine wave with quadratic curve for thinner appearance
        uint16_t val = sin8(phase);
        uint8_t brightness = (val * val) >> 8;

        matrix->drawPixel(i, brightness);
    }

    matrix->show();
    timeBase += speed;
}