/**
 * @file RainEffect.cpp
 * @brief Implementation of the falling raindrop animation effect.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "RainEffect.h"

RainEffect::RainEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                       uint8_t count, uint16_t speed, uint8_t fade)
    : Effect(displayMatrix, ledMapper), numDrops(count > ABSOLUTE_MAX_DROPS ? ABSOLUTE_MAX_DROPS : count),
      baseSpeed(speed), tailFade(fade) {
    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;
}

void RainEffect::setupEffect() {
    for (uint8_t i = 0; i < numDrops; i++) {
        drops[i].col = fast_rand() % 11;
        drops[i].yPos = (fast_rand() % 12) << 8;
        drops[i].speed = baseSpeed + (fast_rand() % SPEED_VARIANCE);
    }

    for (uint8_t i = 0; i < 79; i++) {
        buffer[i] = 0;
        matrix->drawPixel(i, 0);
    }
    matrix->show();
}

void RainEffect::loopEffect() {
    // Fade out existing trails
    for (uint8_t i = 0; i < 79; i++) {
        buffer[i] = (buffer[i] > tailFade) ? (buffer[i] - tailFade) : 0;
    }

    // Update drop physics and draw anti-aliased drops
    for (uint8_t i = 0; i < numDrops; i++) {
        drops[i].yPos += drops[i].speed;

        // Reset when drop reaches bottom
        if (drops[i].yPos > (11 << 8)) {
            drops[i].col = fast_rand() % 11;
            drops[i].yPos = 0;
            drops[i].speed = baseSpeed + (fast_rand() % SPEED_VARIANCE);
        }

        // Interpolate between two rows for smooth sub-pixel motion
        uint8_t y_int = drops[i].yPos >> 8;
        uint8_t frac = drops[i].yPos & 0xFF;

        uint8_t idx_lead = mapper->getIndex(drops[i].col, y_int + 1);
        if (idx_lead != 255) {
            if (frac > buffer[idx_lead]) buffer[idx_lead] = frac;
        }

        uint8_t idx_trail = mapper->getIndex(drops[i].col, y_int);
        if (idx_trail != 255) {
            uint8_t brightness = 255 - frac;
            if (brightness > buffer[idx_trail]) buffer[idx_trail] = brightness;
        }
    }

    for (uint8_t i = 0; i < 79; i++) {
        matrix->drawPixel(i, buffer[i]);
    }
    matrix->show();
}