/**
 * @file Effect.h
 * @brief Base class for all LED matrix effects.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Defines the abstract interface that all visual effects must implement.
 */

#ifndef HEARTLEDMATRIX_EFFECT_H
#define HEARTLEDMATRIX_EFFECT_H

#include <Arduino.h>
#include "matrix/IS31FL3731.h"
#include "matrix/LedMapper.h"

/**
 * @class Effect
 * @brief Abstract base class for LED matrix animation effects.
 *
 * All concrete effects inherit from this class and must implement
 * setupEffect() and loopEffect() to define their animation behavior.
 */
class Effect {
protected:
    IS31FL3731* matrix;  ///< Pointer to the LED matrix driver
    LedMapper* mapper;   ///< Pointer to the LED coordinate mapper

public:
    /**
     * @brief Construct an Effect with references to the display hardware.
     * @param displayMatrix Pointer to the IS31FL3731 LED driver
     * @param ledMapper Pointer to the LedMapper for coordinate translation
     */
    Effect(IS31FL3731* displayMatrix, LedMapper* ledMapper) {
        this->matrix = displayMatrix;
        this->mapper = ledMapper;
    }

    /// Virtual destructor for proper cleanup of derived classes
    virtual ~Effect() {}

    /**
     * @brief Initialize the effect. Called once when the effect is first selected.
     */
    virtual void setupEffect() = 0;

    /**
     * @brief Update and render one frame of animation. Called every loop cycle.
     */
    virtual void loopEffect() = 0;
};

#endif // HEARTLEDMATRIX_EFFECT_H