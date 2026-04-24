/**
 * @file LedMapper.h
 * @brief LED coordinate mapping and lookup system.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Provides efficient bidirectional mapping between 2D grid coordinates
 * and flat LED indices, supporting non-rectangular LED layouts.
 */

#ifndef HEARTLEDMATRIX_LEDMAPPER_H
#define HEARTLEDMATRIX_LEDMAPPER_H

#include <Arduino.h>
#include <cstdint>

/**
 * @class LedMapper
 * @brief Maps 2D coordinates to LED indices for non-rectangular arrays.
 *
 * Uses static arrays to avoid dynamic allocation. Supports fast lookup
 * from grid coordinates to LED index and vice versa.
 */
class LedMapper {
public:
    /**
     * @struct Coordinate
     * @brief A 2D grid position.
     */
    struct Coordinate {
        uint8_t x;  ///< X coordinate (column)
        uint8_t y;  ///< Y coordinate (row)
    };

    static constexpr uint8_t MAX_WIDTH = 11;   ///< Maximum grid width
    static constexpr uint8_t MAX_HEIGHT = 11;  ///< Maximum grid height
    static constexpr uint8_t MAX_LEDS = 80;    ///< Maximum LED count

    uint8_t width;             ///< Actual grid width
    uint8_t height;            ///< Actual grid height
    uint8_t totalLEDs;         ///< Total number of LEDs

    bool presentMap[MAX_WIDTH * MAX_HEIGHT];      ///< Which grid positions have LEDs
    uint8_t coordToIndex[MAX_WIDTH * MAX_HEIGHT]; ///< Map grid coords to LED indices
    Coordinate indexToCoord[MAX_LEDS];            ///< Map LED indices to grid coords

    /**
     * @brief Construct a mapper from an array of LED coordinates.
     * @param coordArray Array of Coordinate positions for each LED
     * @param count Number of LEDs in the array
     */
    LedMapper(Coordinate* coordArray, uint8_t count);

    /**
     * @brief Check if a grid position contains an LED.
     * @param x X coordinate
     * @param y Y coordinate
     * @return True if an LED exists at this position
     */
    bool isPresent(uint8_t x, uint8_t y) const;

    /**
     * @brief Get the LED index for a given coordinate.
     * @param x X coordinate
     * @param y Y coordinate
     * @return LED index (0-79), or 255 if no LED at this position
     */
    uint8_t getIndex(uint8_t x, uint8_t y) const;

    /**
     * @brief Get the grid coordinates for a given LED index.
     * @param index LED index (0-79)
     * @return Coordinate at this LED index, or {255, 255} if invalid
     */
    Coordinate getCoord(uint8_t index) const;
};

#endif // HEARTLEDMATRIX_LEDMAPPER_H