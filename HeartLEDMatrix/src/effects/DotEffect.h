#ifndef CH32TEST_DOTEFFECT_H
#define CH32TEST_DOTEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

class DotEffect : public Effect {
private:
    // Define an absolute ceiling for memory allocation
    static constexpr uint8_t ABSOLUTE_MAX_DOTS = 10;

    // Hardcoded variations
    static constexpr uint16_t RADIUS_VAR = 0;
    static constexpr uint8_t SPEED_VAR = 3;

    struct Dot {
        uint8_t cx, cy;
        uint16_t radius;
        int16_t brightness;
        uint8_t fadeSpeed;
        uint8_t state;     // 0=Dead, 1=Fading In, 2=Fading Out
    };

    // State memory for this specific effect
    Dot dots[ABSOLUTE_MAX_DOTS];
    uint8_t buffer[79];

    // Configurable parameters
    uint8_t activeDots;
    uint16_t baseRadius;
    uint8_t baseSpeed;

public:
    // Constructor declaration
    DotEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
              uint8_t numDots, uint16_t dotSize, uint8_t speed);

    // Override declarations
    void setupEffect() override;
    void loopEffect() override;
};

#endif // CH32TEST_DOTEFFECT_H