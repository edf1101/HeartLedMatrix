/**
 * @file WaveEffect.h
 * @brief Traveling sinusoidal wave effect.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Creates waves that travel across the LED array with optional rotation.
 */

#ifndef HEARTLEDMATRIX_WAVEEFFECT_H
#define HEARTLEDMATRIX_WAVEEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class WaveEffect
 * @brief Animated sinusoidal wave traveling in a specified direction.
 *
 * Creates smooth waves that propagate across the LED matrix,
 * with optional rotation for dynamic effects.
 */
class WaveEffect : public Effect {
private:
    uint16_t timeBase;      ///< Animation timer for wave propagation
    uint16_t currentAngle;  ///< Current direction angle in degrees

    uint8_t speed;          ///< Wave propagation speed per frame
    uint8_t waveLength;     ///< Wavelength (higher = more compressed)
    uint8_t rotationSpeed;  ///< Rotation speed in degrees per frame
    int8_t dirX;            ///< Direction X component (precomputed)
    int8_t dirY;            ///< Direction Y component (precomputed)

public:
    /**
     * @brief Construct a WaveEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param angleDeg Initial wave direction in degrees (0-360)
     * @param waveLength Wavelength parameter (higher = shorter waves)
     * @param speed Wave propagation speed per frame
     * @param rotSpeed Rotation speed in degrees per frame (0 = no rotation)
     */
    WaveEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
               uint16_t angleDeg, uint8_t waveLength, uint8_t speed, uint8_t rotSpeed);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // HEARTLEDMATRIX_WAVEEFFECT_H