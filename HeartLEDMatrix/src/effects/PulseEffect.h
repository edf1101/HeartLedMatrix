/**
 * @file PulseEffect.h
 * @brief Expanding pulse wave that follows the LED shape.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Creates a pulsing wave that expands and contracts from the center,
 * following the natural shape of the LED arrangement.
 */

#ifndef HEARTLEDMATRIX_PULSEEFFECT_H
#define HEARTLEDMATRIX_PULSEEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class PulseEffect
 * @brief Expanding and contracting pulse wave effect.
 *
 * Uses a precomputed signed distance field (SDF) to create a smooth
 * expanding pulse that respects the shape of the LED array.
 */
class PulseEffect : public Effect {
private:
    uint16_t shapeDist[79];  ///< Precomputed distance-from-edge for each LED
    uint16_t maxDist;        ///< Maximum distance in the SDF
    uint16_t currentRadius;  ///< Current pulse wave radius

    uint16_t speed;          ///< Expansion speed per frame
    uint8_t thickness;       ///< Pulse thickness (higher = sharper)
    bool initializedSDF;     ///< Whether SDF has been precomputed

public:
    /**
     * @brief Construct a PulseEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param pulseSpeed Speed of pulse expansion
     * @param pulseThickness Thickness/sharpness of the pulse wave
     */
    PulseEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
                uint16_t pulseSpeed, uint8_t pulseThickness);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // HEARTLEDMATRIX_PULSEEFFECT_H