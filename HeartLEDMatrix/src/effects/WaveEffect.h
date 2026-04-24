#ifndef CH32TEST_WAVEEFFECT_H
#define CH32TEST_WAVEEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

class WaveEffect : public Effect {
private:
    uint16_t timeBase;
    uint16_t currentAngle; // Stored in degrees

    // Configurable parameters
    uint8_t speed;
    uint8_t waveLength;
    uint8_t rotationSpeed; // Degrees per frame
    int8_t dirX;
    int8_t dirY;

    // Helper to update the direction vectors based on currentAngle
    void updateDirection();

public:
    WaveEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
               uint16_t angleDeg, uint8_t waveLength, uint8_t speed, uint8_t rotSpeed);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // CH32TEST_WAVEEFFECT_H