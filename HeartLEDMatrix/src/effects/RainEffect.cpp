#include "RainEffect.h"

RainEffect::RainEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper, 
                       uint8_t count, uint16_t speed, uint8_t fade)
    : Effect(displayMatrix, ledMapper) 
{
    this->numDrops = (count > ABSOLUTE_MAX_DROPS) ? ABSOLUTE_MAX_DROPS : count;
    this->baseSpeed = speed;
    this->tailFade = fade;

    for (uint8_t i = 0; i < 79; i++) buffer[i] = 0;
}

void RainEffect::setupEffect() {
    // Initialize or Reset drops
    for (uint8_t i = 0; i < numDrops; i++) {
        drops[i].col = fast_rand() % 11;
        drops[i].yPos = (fast_rand() % 12) << 8;
        drops[i].speed = baseSpeed + (fast_rand() % SPEED_VARIANCE);
    }
    
    // Clear buffer and hardware
    for (uint8_t i = 0; i < 79; i++) {
        buffer[i] = 0;
        matrix->drawPixel(i, 0);
    }
    matrix->show();
}

void RainEffect::loopEffect() {
    // 1. DIM THE TRAILS
    for (uint8_t i = 0; i < 79; i++) {
        if (buffer[i] > tailFade) {
            buffer[i] -= tailFade;
        } else {
            buffer[i] = 0;
        }
    }

    // 2. UPDATE PHYSICS & DRAW
    for (uint8_t i = 0; i < numDrops; i++) {
        // Physics: Fall down
        drops[i].yPos += drops[i].speed;

        // Reset if off bottom (Y > 11)
        if (drops[i].yPos > (11 << 8)) {
            drops[i].col = fast_rand() % 11;
            drops[i].yPos = 0;
            drops[i].speed = baseSpeed + (fast_rand() % SPEED_VARIANCE);
        }

        // Anti-aliasing
        uint8_t y_int = drops[i].yPos >> 8;
        uint8_t frac = drops[i].yPos & 0xFF;

        // Leading pixel
        uint8_t idx_lead = mapper->getIndex(drops[i].col, y_int + 1);
        if (idx_lead != 255) {
            uint8_t brightness = frac;
            if (brightness > buffer[idx_lead]) buffer[idx_lead] = brightness;
        }

        // Trailing pixel
        uint8_t idx_trail = mapper->getIndex(drops[i].col, y_int);
        if (idx_trail != 255) {
            uint8_t brightness = 255 - frac;
            if (brightness > buffer[idx_trail]) buffer[idx_trail] = brightness;
        }
    }

    // 3. RENDER
    for (uint8_t i = 0; i < 79; i++) {
        matrix->drawPixel(i, buffer[i]);
    }
    matrix->show();
}