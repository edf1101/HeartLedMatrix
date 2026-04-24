#include "DotEffect.h"

// ---------------------------------------------------------
// CONSTRUCTOR
// ---------------------------------------------------------
DotEffect::DotEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper, 
                     uint8_t numDots, uint16_t dotSize, uint8_t speed) 
    : Effect(displayMatrix, ledMapper) // Pass hardware to the base class
{
    // Clamp the requested dots to our static memory limit
    this->activeDots = (numDots > ABSOLUTE_MAX_DOTS) ? ABSOLUTE_MAX_DOTS : numDots;
    this->baseRadius = dotSize;
    this->baseSpeed = speed;
}

// ---------------------------------------------------------
// SETUP
// ---------------------------------------------------------
void DotEffect::setupEffect() {
    // Clear the screen buffer and kill all dots so it starts fresh
    for (uint8_t i = 0; i < 79; i++) {
        buffer[i] = 0;
        matrix->drawPixel(i, 0); 
    }
    for (uint8_t i = 0; i < ABSOLUTE_MAX_DOTS; i++) {
        dots[i].state = 0;
    }
    matrix->show();
}

// ---------------------------------------------------------
// LOOP
// ---------------------------------------------------------
void DotEffect::loopEffect() {
    // ==========================================
    // 1. SPAWN DEAD DOTS
    // ==========================================
    for (uint8_t d = 0; d < activeDots; d++) {
        if (dots[d].state == 0) {
            uint8_t centerLed = fast_rand() % 79;
            LedMapper::Coordinate c = mapper->getCoord(centerLed);
            dots[d].cx = c.x;
            dots[d].cy = c.y;

            // Safe Radius Math
            dots[d].radius = baseRadius;
            if (RADIUS_VAR > 0) {
                uint16_t rVar = 0;
                if (RADIUS_VAR > 0) {
                    rVar = fast_rand() % (RADIUS_VAR * 2);
                }
                dots[d].radius = (baseRadius - RADIUS_VAR) + rVar;
            }

            // Safe Speed Math
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

    // ==========================================
    // 2. UPDATE FADES & CLEAR BUFFER
    // ==========================================
    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;

    for (uint8_t d = 0; d < activeDots; d++) {
        if (dots[d].state == 1) { 
            dots[d].brightness += dots[d].fadeSpeed;
            if (dots[d].brightness >= 255) {
                dots[d].brightness = 255;
                dots[d].state = 2; 
            }
        }
        else if (dots[d].state == 2) { 
            dots[d].brightness -= dots[d].fadeSpeed;
            if (dots[d].brightness <= 0) {
                dots[d].brightness = 0;
                dots[d].state = 0; 
            }
        }
    }

    // ==========================================
    // 3. DRAW DOTS (TRUE EUCLIDEAN DISTANCE)
    // ==========================================
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
                uint16_t spatialBrightness = 255;

                if (distToEdge < 256) {
                    spatialBrightness = distToEdge;
                }

                uint16_t finalBrightness = (spatialBrightness * dots[d].brightness) >> 8;

                if (finalBrightness > buffer[i]) {
                    buffer[i] = finalBrightness;
                }
            }
        }
    }

    // ==========================================
    // 4. RENDER TO MATRIX
    // ==========================================
    for (uint8_t i = 0; i < 79; i++) {
        matrix->drawPixel(i, buffer[i]);
    }
    matrix->show();
}