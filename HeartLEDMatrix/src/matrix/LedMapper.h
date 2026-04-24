#ifndef CH32TEST_LEDMAPPER_H
#define CH32TEST_LEDMAPPER_H

#include <Arduino.h>
#include <cstdint>

class LedMapper {
public:
    struct Coordinate {
        uint8_t x;
        uint8_t y;
    };

    // Absolute maximums to satisfy the compiler's need for static sizing
    static constexpr uint8_t MAX_WIDTH = 11;
    static constexpr uint8_t MAX_HEIGHT = 11;
    static constexpr uint8_t MAX_LEDS = 80;

    uint8_t width;
    uint8_t height;
    uint8_t totalLEDs;

    // Statically allocated arrays (Bypasses malloc entirely!)
    bool presentMap[MAX_WIDTH * MAX_HEIGHT];
    uint8_t coordToIndex[MAX_WIDTH * MAX_HEIGHT];
    Coordinate indexToCoord[MAX_LEDS];

    // Constructor
    LedMapper(Coordinate* coordArray, uint8_t count);

    // Helpers
    bool isPresent(uint8_t x, uint8_t y) const;
    uint8_t getIndex(uint8_t x, uint8_t y) const;
    Coordinate getCoord(uint8_t index) const;
};

#endif //CH32TEST_LEDMAPPER_H