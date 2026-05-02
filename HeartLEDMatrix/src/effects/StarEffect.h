/**
 * @file StarEffect.h
 * @brief Shooting star effect for LED animations.
 * @author Ed Fillingham
 * @date May 2026
 *
 * Creates multiple shooting stars that burst from the centre, accelerate
 * outward, and render as motion-blurred trails with tapered brightness.
 */

#ifndef HEARTLEDMATRIX_STAREFFECT_H
#define HEARTLEDMATRIX_STAREFFECT_H

#include <stdint.h>
#include <stdlib.h>
#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class StarEffect
 * @brief Animated shooting stars radiating from the centre with tapered brightness.
 *
 * Spawns stars at the display centre, accelerates them outward in random
 * directions, and renders each as a motion-blurred trail with a double-taper
 * fade at head and tail.
 */
class StarEffect : public Effect {
private:
    static constexpr uint8_t ABSOLUTE_MAX_STARS = 12;  ///< Maximum concurrent stars

    /**
     * @struct Star
     * @brief A single animated shooting star.
     */
    struct Star {
        int16_t x, y;           ///< Current position in 8.8 fixed-point
        int8_t dirX, dirY;      ///< Normalised direction vector
        uint16_t curS;          ///< Current speed in 8.8 fixed-point
        uint16_t targetS;       ///< Target (maximum) speed
        uint8_t samples;        ///< Trail length in sub-pixel samples
        uint16_t distTraveled;  ///< Distance from spawn in 8.8 fixed-point
        bool active;            ///< Whether this star slot is in use
    };

    Star stars[ABSOLUTE_MAX_STARS];  ///< Active star instances
    uint8_t buffer[79];              ///< Per-LED brightness buffer

    uint8_t maxStars;    ///< Number of stars to animate simultaneously
    uint16_t baseSpeed;  ///< Base travel speed
    uint16_t speedVar;   ///< Speed variation range
    uint8_t baseLength;  ///< Base trail length in pixels
    uint8_t lengthVar;   ///< Trail length variation range

public:
    /**
     * @brief Construct a StarEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param count Number of simultaneous stars
     * @param speed Base travel speed (8.8 fixed-point)
     * @param speedVar Speed variation range
     * @param length Base trail length in pixels
     * @param lengthVar Trail length variation range
     */
    StarEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
               uint8_t count, uint16_t speed, uint16_t speedVar,
               uint8_t length, uint8_t lengthVar);

    void setupEffect() override;
    void loopEffect() override;
    void spawnStar(uint8_t i);
};

#endif // HEARTLEDMATRIX_STAREFFECT_H