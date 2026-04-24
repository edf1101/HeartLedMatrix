#include "WaveEffect.h"

WaveEffect::WaveEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                       uint16_t angleDeg, uint8_t waveLength, uint8_t speed, uint8_t rotSpeed)
    : Effect(displayMatrix, ledMapper)
{
    this->speed = speed;
    this->waveLength = waveLength;
    this->timeBase = 0;
    this->rotationSpeed = rotSpeed;

    // Pre-calculate direction vectors once to save CPU time in the loop
    // Uses the utility logic to map 360 degrees to 256 steps
    uint8_t angle8 = (uint8_t)((angleDeg % 360 * 32) / 45);
    this->dirX = (int8_t)cos8(angle8) - 128;
    this->dirY = (int8_t)sin8(angle8) - 128;
}

void WaveEffect::setupEffect()
{
    timeBase = 0;
    for (uint8_t i = 0; i < 79; i++) matrix->drawPixel(i, 0);
    matrix->show();
}

void WaveEffect::loopEffect()
{
    if (rotationSpeed != 0)
    {
        currentAngle += rotationSpeed;
        uint8_t angle8 = (uint8_t)((currentAngle % 360 * 32) / 45);
        this->dirX = (int8_t)cos8(angle8) - 128;
        this->dirY = (int8_t)sin8(angle8) - 128;
    }

    for (uint8_t i = 0; i < 79; i++)
    {
        LedMapper::Coordinate c = mapper->getCoord(i);
        if (c.x == 255) continue;

        // 1. Calculate spatial offset based on pre-calculated direction
        int16_t spatialOffset = (c.x * dirX) + (c.y * dirY);
        spatialOffset = (spatialOffset * waveLength) / 128;

        // 2. Determine phase (auto-wraps via uint8_t cast)
        uint8_t phase = (uint8_t)(spatialOffset - (uint8_t)timeBase);

        // 3. Get brightness and apply the Quadratic Curve for a "thinner" look
        uint16_t val = sin8(phase);
        uint8_t brightness = (val * val) >> 8;

        matrix->drawPixel(i, brightness);
    }

    matrix->show();
    timeBase += speed;
}
