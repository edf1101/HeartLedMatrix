/**
 * @file WaveEffect.cpp
 * @brief Implementation of the traveling wave animation effect.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "WaveEffect.h"

WaveEffect::WaveEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                       uint16_t angleDeg, uint8_t waveLength, uint8_t speed, uint8_t rotSpeed)
    : Effect(displayMatrix, ledMapper), speed(speed), waveLength(waveLength),
      timeBase(0), rotationSpeed(rotSpeed), currentAngle(angleDeg) {
    uint8_t angle8 = (uint8_t)((angleDeg % 360 * 32) / 45);
    dirX = (int8_t)cos8(angle8) - 128;
    dirY = (int8_t)sin8(angle8) - 128;
}

void WaveEffect::setupEffect() {
    timeBase = 0;
    for (uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
    matrix->show();
}

void WaveEffect::loopEffect() {
    if (rotationSpeed != 0) {
        currentAngle += rotationSpeed;
        uint8_t angle8 = (uint8_t)((currentAngle % 360 * 32) / 45);
        dirX = (int8_t)cos8(angle8) - 128;
        dirY = (int8_t)sin8(angle8) - 128;
    }

    for (uint8_t i = 0; i < 79; i++) {
        LedMapper::Coordinate c = mapper->getCoord(i);
        if (c.x == 255) continue;

        // Project position onto wave direction to get phase
        int16_t spatialOffset = (c.x * dirX) + (c.y * dirY);
        spatialOffset = (spatialOffset * waveLength) / 128;

        uint8_t phase = (uint8_t)(spatialOffset - (uint8_t)timeBase);

        // Apply sine wave with quadratic curve for thinner appearance
        uint16_t val = sin8(phase);
        uint8_t brightness = (val * val) >> 8;

        matrix->drawPixel(i, brightness);
    }

    matrix->show();
    timeBase += speed;
}
