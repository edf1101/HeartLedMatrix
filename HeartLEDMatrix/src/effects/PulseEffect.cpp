#include "PulseEffect.h"

PulseEffect::PulseEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper, 
                         uint16_t pulseSpeed, uint8_t pulseThickness)
    : Effect(displayMatrix, ledMapper)
{
    this->speed = pulseSpeed;
    this->thickness = pulseThickness;
    this->currentRadius = 0;
    this->initializedSDF = false;
    this->maxDist = 0;

    // Clear the internal array
    for(uint8_t i = 0; i < 79; i++) shapeDist[i] = 0;
}

void PulseEffect::setupEffect() {
    currentRadius = 0;

    // Perform the one-time SDF Precalculation
    if (!initializedSDF) {
        maxDist = 0;
        uint16_t edgeDist[79] = {0};

        // For every LED on the board...
        for (uint8_t i = 0; i < 79; i++) {
            LedMapper::Coordinate c = mapper->getCoord(i);
            uint16_t minDist = 65535;

            // Brute force scan the entire grid + a 1-pixel outer border
            // We use the mapper's dimensions to find "empty space"
            for (int8_t y = -1; y <= mapper->height; y++) {
                for (int8_t x = -1; x <= mapper->width; x++) {

                    // If this spot is empty space or off the edge of the board
                    if (!mapper->isPresent((uint8_t)x, (uint8_t)y)) {
                        int8_t dx = c.x - x;
                        int8_t dy = c.y - y;

                        // Calculate distance to this empty space using our fast math
                        uint16_t d = dist_scaled(dx, dy) << 5;

                        // Keep the smallest distance (the nearest edge!)
                        if (d < minDist) minDist = d;
                    }
                }
            }
            edgeDist[i] = minDist;
            if (minDist > maxDist) maxDist = minDist;
        }

        // Invert the map so center is 0
        for (uint8_t i = 0; i < 79; i++) {
            shapeDist[i] = maxDist - edgeDist[i];
        }

        initializedSDF = true;
    }

    // Clear hardware display buffer
    for(uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
    matrix->show();
}

void PulseEffect::loopEffect() {
    // ==========================================
    // DRAW THE FRAME
    // ==========================================
    for (uint8_t i = 0; i < 79; i++) {
        uint16_t ledDist = shapeDist[i];

        uint16_t diff = 0;
        if (ledDist > currentRadius) {
            diff = ledDist - currentRadius;
        } else {
            diff = currentRadius - ledDist;
        }

        // Anti-aliasing fade
        uint16_t fade = (diff * thickness) >> 4;

        uint8_t brightness = 0;
        if (fade < 255) {
            brightness = 255 - fade;
        }

        matrix->drawPixel(i, brightness);
    }

    matrix->show();

    currentRadius += speed;

    // Reset with pause logic
    if (currentRadius > maxDist + 400) {
        currentRadius = 0;
    }
}