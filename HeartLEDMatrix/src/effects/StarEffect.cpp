/**
 * @file StarEffect.cpp
 * @brief Implementation of the shooting star animation effect.
 * @author Ed Fillingham
 * @date May 2026
 */

#include "StarEffect.h"

StarEffect::StarEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                       uint8_t count, uint16_t speed, uint16_t speedVar,
                       uint8_t length, uint8_t lengthVar)
    : Effect(displayMatrix, ledMapper)
{
    this->maxStars = (count > ABSOLUTE_MAX_STARS) ? ABSOLUTE_MAX_STARS : count;
    this->baseSpeed = speed;
    this->speedVar = speedVar;
    this->baseLength = length;
    this->lengthVar = lengthVar;
}

void StarEffect::spawnStar(uint8_t i) {
    stars[i].active = true;
    stars[i].x = 1280; // Center (5.0 in 8.8)
    stars[i].y = 1280;

    uint8_t angle = (uint8_t)(fast_rand() & 0xFF);
    stars[i].dirX = (int16_t)cos8(angle) - 128;
    stars[i].dirY = (int16_t)sin8(angle) - 128;

    uint16_t target = baseSpeed + (fast_rand() % speedVar);
    stars[i].targetS = target;
    stars[i].curS = target / 10;

    // NEW: Reset distance tracker
    stars[i].distTraveled = 0;

    uint8_t pixelLen = baseLength + (fast_rand() % (lengthVar + 1));
    stars[i].samples = (pixelLen < 1) ? 4 : (pixelLen * 4);
}

void StarEffect::setupEffect() {
    for (uint8_t i = 0; i < ABSOLUTE_MAX_STARS; i++) stars[i].active = false;
    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;
}
void StarEffect::loopEffect() {
    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;

    for (uint8_t i = 0; i < maxStars; i++) {
        if (!stars[i].active) {
            if ((fast_rand() % 40) == 0) spawnStar(i);
            continue;
        }

        // 1. PHYSICS & DISTANCE
        if (stars[i].curS < stars[i].targetS) {
            stars[i].curS += (stars[i].curS >> 5) + 1;
        }
        stars[i].distTraveled += stars[i].curS;

        int16_t vx = ((int32_t)stars[i].dirX * stars[i].curS) >> 7;
        int16_t vy = ((int32_t)stars[i].dirY * stars[i].curS) >> 7;
        stars[i].x += vx;
        stars[i].y += vy;

        int16_t stepX = (int16_t)stars[i].dirX >> 1;
        int16_t stepY = (int16_t)stars[i].dirY >> 1;

        // 2. ORGANIC GROWTH
        uint16_t allowedSamples = (stars[i].distTraveled >> 6) + 1;
        uint8_t actualSamples = (allowedSamples > stars[i].samples) ? stars[i].samples : (uint8_t)allowedSamples;

        // 3. BOUNDARY CHECK
        int16_t tailX = stars[i].x - (stepX * (actualSamples - 1));
        int16_t tailY = stars[i].y - (stepY * (actualSamples - 1));

        int16_t dx = abs(tailX - 1280);
        int16_t dy = abs(tailY - 1280);
        if (dx > 2048 || dy > 2048) {
            stars[i].active = false;
            continue;
        }

        // --- NEW: PIECE-WISE FADE LOGIC ---
        // Calculate the 25% threshold length (fadeLen). bitshift >> 2 is divide by 4.
        uint8_t fadeLen = stars[i].samples >> 2;
        if (fadeLen == 0) fadeLen = 1; // Prevent division by zero on tiny stars

        // Calculate the brightness step-per-sample outside the loop to save CPU cycles
        uint16_t fadeStep = 255 / fadeLen;

        // 4. DRAWING LOOP
        for (uint8_t s = 0; s < actualSamples; s++) {
            int16_t px = stars[i].x - (stepX * s);
            int16_t py = stars[i].y - (stepY * s);

            uint8_t ix = px >> 8;
            uint8_t iy = py >> 8;
            uint16_t fx = px & 0xFF;
            uint16_t fy = py & 0xFF;

            // Apply the double-taper intensity
            uint16_t intensity = 255;
            if (s < fadeLen) {
                // Front 25% (Head) fading in from 0 -> 255
                intensity = s * fadeStep;
            } else if (s >= stars[i].samples - fadeLen) {
                // Back 25% (Tail) fading out from 255 -> 0
                uint8_t tailDist = (stars[i].samples - 1) - s;
                intensity = tailDist * fadeStep;
            }

            uint16_t wTL = ((256 - fx) * (256 - fy)) >> 8;
            uint16_t wTR = (fx * (256 - fy)) >> 8;
            uint16_t wBL = ((256 - fx) * fy) >> 8;
            uint16_t wBR = (fx * fy) >> 8;

            auto drawNeedle = [&](uint8_t x, uint8_t y, uint16_t weight) {
                if (weight < 32) return;
                uint8_t idx = mapper->getIndex(x, y);
                if (idx != 255) {
                    uint32_t cubed = (uint32_t)weight * weight * weight;
                    uint16_t finalB = ((uint32_t)intensity * (cubed >> 16)) >> 8;
                    if (finalB > buffer[idx]) buffer[idx] = (uint8_t)finalB;
                }
            };

            drawNeedle(ix,     iy,     wTL);
            drawNeedle(ix + 1, iy,     wTR);
            drawNeedle(ix,     iy + 1, wBL);
            drawNeedle(ix + 1, iy + 1, wBR);
        }
    }

    for (uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, buffer[i]);
    matrix->show();
}