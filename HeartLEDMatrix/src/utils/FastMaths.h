/**
 * @file FastMaths.h
 * @brief Fast approximation functions for LED animation math.
 * @author Ed Fillingham
 * @date April 2026
 *
 * Provides optimized implementations of trigonometry, distance, and random number
 * generation suitable for embedded systems without floating-point hardware.
 */

#ifndef HEARTLEDMATRIX_FASTMATHS_H
#define HEARTLEDMATRIX_FASTMATHS_H

#include <stdint.h>

/// Precomputed sine lookup table (0-255 scale) for fast trigonometry
const uint8_t sin8_lut[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
    218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
    245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
    255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246,
    245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
    218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
    128, 124, 121, 118, 115, 112, 109, 106, 103, 100,  97,  93,  90,  88,  85,  82,
     79,  76,  73,  70,  67,  65,  62,  59,  57,  54,  52,  49,  47,  44,  42,  40,
     37,  35,  33,  31,  29,  27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,
     10,   9,   7,   6,   5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,
      0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,
     10,  11,  12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,
     37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,  73,  76,
     79,  82,  85,  88,  90,  93,  97, 100, 103, 106, 109, 112, 115, 118, 121, 124
};

/**
 * @defgroup FastMath8 Raw 8-Bit Math Functions
 * @brief Fast trigonometry using 8-bit angle scale (0-255 = 0-360°).
 * @{
 */

/// Get sine of an 8-bit angle (0-255 = 0-360°)
inline uint8_t sin8(uint8_t angle8) {
    return sin8_lut[angle8];
}

/// Get cosine of an 8-bit angle (0-255 = 0-360°)
inline uint8_t cos8(uint8_t angle8) {
    return sin8_lut[(uint8_t)(angle8 + 64)];
}

/** @} */

/**
 * @defgroup FastMathDeg Degree-Based Math Functions
 * @brief Trigonometry using degrees (0-360°).
 * @{
 */

/// Get sine of an angle in degrees
inline uint8_t sin_deg(uint16_t degrees) {
    degrees = degrees % 360;
    uint8_t angle8 = (uint8_t)((degrees * 32) / 45);
    return sin8_lut[angle8];
}

/// Get cosine of an angle in degrees
inline uint8_t cos_deg(uint16_t degrees) {
    degrees = degrees % 360;
    uint8_t angle8 = (uint8_t)((degrees * 32) / 45);
    return sin8_lut[(uint8_t)(angle8 + 64)];
}

/** @} */

/// Absolute value for signed 8-bit integers
inline uint8_t abs8(int8_t x) {
    return x < 0 ? -x : x;
}

/**
 * @brief Fast arctangent of dy/dx, returning an 8-bit angle (0-255 = 0-360°).
 * @param dy Y component of the vector
 * @param dx X component of the vector
 * @return Angle as 8-bit value (0-255 representing 0-360°)
 */
inline uint8_t atan2_8(int8_t dy, int8_t dx) {
    if (dx == 0 && dy == 0) return 0;

    uint8_t abs_y = abs8(dy);
    uint8_t abs_x = abs8(dx);
    uint8_t angle;

    if (abs_x >= abs_y) {
        angle = (32 * abs_y) / abs_x;
    } else {
        angle = 64 - ((32 * abs_x) / abs_y);
    }

    if (dx < 0) {
        if (dy < 0) return 128 + angle;
        else return 128 - angle;
    } else {
        if (dy < 0) return 256 - angle;
        else return angle;
    }
}

/**
 * @brief Fast distance approximation using octagonal norm, scaled by ~8x.
 *
 * Approximates sqrt(dx² + dy²) using integer math. Scaled up by 8x
 * to provide sub-pixel precision for smooth gradients.
 *
 * @param dx X distance
 * @param dy Y distance
 * @return Scaled distance estimate
 */
inline uint8_t dist_scaled(int8_t dx, int8_t dy) {
    uint8_t abs_x = abs8(dx);
    uint8_t abs_y = abs8(dy);
    uint8_t min_val = (abs_x < abs_y) ? abs_x : abs_y;
    uint8_t max_val = (abs_x > abs_y) ? abs_x : abs_y;

    return (max_val * 8) + (min_val * 3);
}


/**
 * @defgroup FastMathRandom Random Number Generation
 * @brief Lightweight pseudo-random number generation.
 * @{
 */

static uint32_t _rand_seed = 12345; ///< Current random seed

/// Seed the random number generator
inline void seed_rand(uint32_t s) {
    _rand_seed = s;
}

/// Generate a pseudo-random 16-bit number
inline uint16_t fast_rand() {
    _rand_seed = (_rand_seed * 1103515245 + 12345) & 0x7fffffff;
    return (uint16_t)(_rand_seed >> 16);
}

/** @} */

/**
 * @brief True Euclidean distance using 8.8 fixed-point arithmetic.
 *
 * Computes perfect sqrt(dx² + dy²) using integer-only arithmetic.
 * Result is in 8.8 fixed-point format for sub-pixel accuracy.
 *
 * @param dx X distance
 * @param dy Y distance
 * @return Distance as 8.8 fixed-point value
 */
inline uint16_t true_dist_fixed(int8_t dx, int8_t dy) {
    uint32_t dx32 = abs8(dx) << 8;
    uint32_t dy32 = abs8(dy) << 8;
    uint32_t dist_sq = (dx32 * dx32) + (dy32 * dy32);

    uint32_t root = 0;
    uint32_t bit = 1UL << 30;

    while (bit > dist_sq) bit >>= 2;
    while (bit != 0) {
        if (dist_sq >= root + bit) {
            dist_sq -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return (uint16_t)root;
}

#endif // HEARTLEDMATRIX_FASTMATHS_H