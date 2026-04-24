#ifndef CH32TEST_RAINEFFECT_H
#define CH32TEST_RAINEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

class RainEffect : public Effect {
private:
    static constexpr uint8_t ABSOLUTE_MAX_DROPS = 12;
    static constexpr uint16_t SPEED_VARIANCE = 25;

    struct Drop {
        uint8_t col;
        uint16_t yPos;  // Fixed point 8.8
        uint16_t speed; // Fixed point 8.8
    };

    Drop drops[ABSOLUTE_MAX_DROPS];
    uint8_t buffer[79];

    uint8_t numDrops;
    uint16_t baseSpeed;
    uint8_t tailFade;

public:
    RainEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
               uint8_t count, uint16_t speed, uint8_t fade);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // CH32TEST_RAINEFFECT_H