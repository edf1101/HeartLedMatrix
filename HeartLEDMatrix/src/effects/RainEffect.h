/**
 * @file RainEffect.h
 * @brief Falling raindrops effect.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Simulates raindrops falling down the LED array with anti-aliasing.
 */

#ifndef HEARTLEDMATRIX_RAINEFFECT_H
#define HEARTLEDMATRIX_RAINEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class RainEffect
 * @brief Animated raindrops falling down the LED array.
 *
 * Multiple drops fall vertically with variable speeds, using
 * anti-aliasing for smooth sub-pixel motion.
 */
class RainEffect : public Effect {
private:
    static constexpr uint8_t ABSOLUTE_MAX_DROPS = 12;  ///< Maximum concurrent drops
    static constexpr uint16_t SPEED_VARIANCE = 25;     ///< Speed variation range

    /**
     * @struct Drop
     * @brief A single falling raindrop.
     */
    struct Drop {
        uint8_t col;        ///< Column (X coordinate)
        uint16_t yPos;      ///< Vertical position (8.8 fixed point)
        uint16_t speed;     ///< Fall speed (8.8 fixed point)
    };

    Drop drops[ABSOLUTE_MAX_DROPS]; ///< Active drop instances
    uint8_t buffer[79];             ///< Per-LED brightness buffer

    uint8_t numDrops;   ///< Number of drops to animate
    uint16_t baseSpeed; ///< Base fall speed (8.8 fixed point)
    uint8_t tailFade;   ///< Fade amount per frame for trails

public:
    /**
     * @brief Construct a RainEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param count Number of simultaneous raindrops
     * @param speed Base fall speed (8.8 fixed point)
     * @param fade Trail fade amount per frame
     */
    RainEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
               uint8_t count, uint16_t speed, uint8_t fade);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // HEARTLEDMATRIX_RAINEFFECT_H