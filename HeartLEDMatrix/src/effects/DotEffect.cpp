/**
 * @file DotEffect.cpp
 * @brief Implementation of the expanding dot animation effect.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "DotEffect.h"

DotEffect::DotEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                     uint8_t numDots, uint16_t dotSize, uint8_t speed)
    : Effect(displayMatrix, ledMapper) {
    activeDots = (numDots > ABSOLUTE_MAX_DOTS) ? ABSOLUTE_MAX_DOTS : numDots;
    baseRadius = dotSize;
    baseSpeed = speed;
}

void DotEffect::setupEffect() {
    for (uint8_t i = 0; i < 79; i++) {
        buffer[i] = 0;
        matrix->drawPixel(i, 0);
    }
    for (uint8_t i = 0; i < ABSOLUTE_MAX_DOTS; i++) {
        dots[i].state = 0;
    }
    matrix->show();
}

void DotEffect::loopEffect() {
    // Spawn new dots when they finish their lifecycle
    for (uint8_t d = 0; d < activeDots; d++) {
        if (dots[d].state == 0) {
            uint8_t centerLed = fast_rand() % 79;
            LedMapper::Coordinate c = mapper->getCoord(centerLed);
            dots[d].cx = c.x;
            dots[d].cy = c.y;
            dots[d].radius = baseRadius;

            dots[d].fadeSpeed = baseSpeed;
            if (SPEED_VAR > 0) {
                uint8_t sVar = fast_rand() % (SPEED_VAR * 2);
                dots[d].fadeSpeed = (baseSpeed - SPEED_VAR) + sVar;
            }
            if (dots[d].fadeSpeed == 0) dots[d].fadeSpeed = 1;

            dots[d].brightness = 0;
            dots[d].state = 1;
        }
    }

    // Clear buffer and update brightness fades
    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;

    for (uint8_t d = 0; d < activeDots; d++) {
        if (dots[d].state == 1) {
            dots[d].brightness += dots[d].fadeSpeed;
            if (dots[d].brightness >= 255) {
                dots[d].brightness = 255;
                dots[d].state = 2;
            }
        } else if (dots[d].state == 2) {
            dots[d].brightness -= dots[d].fadeSpeed;
            if (dots[d].brightness <= 0) {
                dots[d].brightness = 0;
                dots[d].state = 0;
            }
        }
    }

    // Render all active dots with perfect circular gradients
    for (uint8_t d = 0; d < activeDots; d++) {
        if (dots[d].state == 0) continue;

        for (uint8_t i = 0; i < 79; i++) {
            LedMapper::Coordinate c = mapper->getCoord(i);
            if (c.x == 255) continue;

            int8_t dx = c.x - dots[d].cx;
            int8_t dy = c.y - dots[d].cy;
            uint16_t dist = true_dist_fixed(dx, dy);

            if (dist < dots[d].radius) {
                uint16_t distToEdge = dots[d].radius - dist;
                uint16_t spatialBrightness = (distToEdge < 256) ? distToEdge : 255;
                uint16_t finalBrightness = (spatialBrightness * dots[d].brightness) >> 8;

                if (finalBrightness > buffer[i]) {
                    buffer[i] = finalBrightness;
                }
            }
        }
    }

    for (uint8_t i = 0; i < 79; i++) {
        matrix->drawPixel(i, buffer[i]);
    }
    matrix->show();
}