/**
 * @file DotEffect.h
 * @brief Expanding/pulsing dot effect for LED animations.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Creates expanding circular dots that fade in and out.
 */

#ifndef HEARTLEDMATRIX_DOTEFFECT_H
#define HEARTLEDMATRIX_DOTEFFECT_H

#include "Effect.h"
#include "utils/FastMaths.h"

/**
 * @class DotEffect
 * @brief Animated expanding circular dots with fade effects.
 *
 * Generates multiple expanding dots at random positions, each fading in
 * and out with smooth radial gradients.
 */
class DotEffect : public Effect {
private:
    static constexpr uint8_t ABSOLUTE_MAX_DOTS = 10;  ///< Maximum concurrent dots
    static constexpr uint16_t RADIUS_VAR = 0;         ///< Radius variation (unused)
    static constexpr uint8_t SPEED_VAR = 3;           ///< Speed variation range

    /**
     * @struct Dot
     * @brief A single animated dot.
     */
    struct Dot {
        uint8_t cx, cy;       ///< Center coordinates
        uint16_t radius;      ///< Expansion radius
        int16_t brightness;   ///< Current brightness (0-255)
        uint8_t fadeSpeed;    ///< Fade speed per frame
        uint8_t state;        ///< 0=Dead, 1=Fading In, 2=Fading Out
    };

    Dot dots[ABSOLUTE_MAX_DOTS]; ///< Active dot instances
    uint8_t buffer[79];          ///< Per-LED brightness buffer

    uint8_t activeDots;   ///< Number of dots to animate
    uint16_t baseRadius;  ///< Base expansion radius
    uint8_t baseSpeed;    ///< Base fade speed

public:
    /**
     * @brief Construct a DotEffect with parameters.
     * @param displayMatrix Pointer to the LED matrix driver
     * @param ledMapper Pointer to the coordinate mapper
     * @param numDots Number of simultaneous dots
     * @param dotSize Base radius for expanding dots
     * @param speed Fade speed (higher = faster fade)
     */
    DotEffect(IS31FL3731* displayMatrix, LedMapper* ledMapper,
              uint8_t numDots, uint16_t dotSize, uint8_t speed);

    void setupEffect() override;
    void loopEffect() override;
};

#endif // HEARTLEDMATRIX_DOTEFFECT_H