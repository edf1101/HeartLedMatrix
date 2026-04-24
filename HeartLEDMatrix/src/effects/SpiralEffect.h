/**
 * @file SpiralEffect.h
 * @brief Multi-armed spiral effect.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Creates rotating spiral patterns emanating from the center of the LED array.
 */

#ifndef HEARTLEDMATRIX_SPIRALEFFECT_H
#define HEARTLEDMATRIX_SPIRALEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class SpiralEffect
 * @brief Rotating spiral wave patterns.
 *
 * Creates one or more spiral arms rotating around the center of the LED array,
 * with adjustable tightness and rotation speed.
 */
class SpiralEffect : public Effect {
private:
    uint16_t timeBase;  ///< Animation timer

    uint8_t speed;      ///< Rotation speed per frame
    uint8_t tightness;  ///< Spiral tightness (higher = more compressed)
    uint8_t arms;       ///< Number of spiral arms

    static constexpr int8_t cx = 5;  ///< Center X coordinate
    static constexpr int8_t cy = 5;  ///< Center Y coordinate

public:
    /**
     * @brief Construct a SpiralEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param rotationSpeed Rotation speed in degrees per frame
     * @param spiralTightness Spiral tightness parameter (higher = tighter)
     * @param numArms Number of spiral arms (1-4 recommended)
     */
    SpiralEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                 uint8_t rotationSpeed, uint8_t spiralTightness, uint8_t numArms);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // HEARTLEDMATRIX_SPIRALEFFECT_H