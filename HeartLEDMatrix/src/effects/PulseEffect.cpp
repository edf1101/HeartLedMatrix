/**
 * @file PulseEffect.cpp
 * @brief Implementation of the pulsing wave animation effect.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "PulseEffect.h"

PulseEffect::PulseEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                         uint16_t pulseSpeed, uint8_t pulseThickness)
    : Effect(displayMatrix, ledMapper), speed(pulseSpeed), thickness(pulseThickness),
      currentRadius(0), initializedSDF(false), maxDist(0) {
    for (uint8_t i = 0; i < 79; i++) shapeDist[i] = 0;
}

void PulseEffect::setupEffect() {
    currentRadius = 0;

    // Compute signed distance field: distance from each LED to the nearest edge
    if (!initializedSDF) {
        maxDist = 0;
        uint16_t edgeDist[79] = {0};

        for (uint8_t i = 0; i < 79; i++) {
            LedMapper::Coordinate c = mapper->getCoord(i);
            uint16_t minDist = 65535;

            // Find the nearest empty space to determine distance-to-edge
            for (int8_t y = -1; y <= mapper->height; y++) {
                for (int8_t x = -1; x <= mapper->width; x++) {
                    if (!mapper->isPresent((uint8_t)x, (uint8_t)y)) {
                        int8_t dx = c.x - x;
                        int8_t dy = c.y - y;
                        uint16_t d = dist_scaled(dx, dy) << 5;
                        if (d < minDist) minDist = d;
                    }
                }
            }
            edgeDist[i] = minDist;
            if (minDist > maxDist) maxDist = minDist;
        }

        // Invert: center (0 distance to edge) gets max value
        for (uint8_t i = 0; i < 79; i++) {
            shapeDist[i] = maxDist - edgeDist[i];
        }

        initializedSDF = true;
    }

    for (uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
    matrix->show();
}

void PulseEffect::loopEffect() {
    // Render the current pulse wave position
    for (uint8_t i = 0; i < 79; i++) {
        uint16_t ledDist = shapeDist[i];
        uint16_t diff = (ledDist > currentRadius) ? (ledDist - currentRadius)
                                                   : (currentRadius - ledDist);

        uint16_t fade = (diff * thickness) >> 4;
        uint8_t brightness = (fade < 255) ? (255 - fade) : 0;

        matrix->drawPixel(i, brightness);
    }

    matrix->show();
    currentRadius += speed;

    // Reset pulse after reaching the edge
    if (currentRadius > maxDist + 400) {
        currentRadius = 0;
    }
}