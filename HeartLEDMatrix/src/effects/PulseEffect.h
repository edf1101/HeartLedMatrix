#ifndef CH32TEST_PULSEEFFECT_H
#define CH32TEST_PULSEEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

class PulseEffect : public Effect {
private:
    uint16_t shapeDist[79];
    uint16_t maxDist;
    uint16_t currentRadius;

    // Configurable parameters
    uint16_t speed;
    uint8_t thickness;
    bool initializedSDF;

public:
    PulseEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                uint16_t pulseSpeed, uint8_t pulseThickness);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // CH32TEST_PULSEEFFECT_H