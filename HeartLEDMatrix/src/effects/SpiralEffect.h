#ifndef CH32TEST_SPIRALEFFECT_H
#define CH32TEST_SPIRALEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

class SpiralEffect : public Effect {
private:
    uint16_t timeBase;

    // Configurable parameters
    uint8_t speed;
    uint8_t tightness;
    uint8_t arms;

    // Center coordinates (D50 is 5,5)
    static constexpr int8_t cx = 5;
    static constexpr int8_t cy = 5;

public:
    SpiralEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                 uint8_t rotationSpeed, uint8_t spiralTightness, uint8_t numArms);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // CH32TEST_SPIRALEFFECT_H