#include "LedMapper.h"

LedMapper::LedMapper(Coordinate* coordArray, uint8_t count) {
    // Safety cap
    totalLEDs = count > MAX_LEDS ? MAX_LEDS : count;

    if (totalLEDs == 0) {
        width = 0; height = 0;
        return;
    }

    uint8_t minX = 255, minY = 255;
    uint8_t maxX = 0, maxY = 0;

    // 1. Find bounds
    for (uint8_t i = 0; i < totalLEDs; i++) {
        if (coordArray[i].x < minX) minX = coordArray[i].x;
        if (coordArray[i].y < minY) minY = coordArray[i].y;
        if (coordArray[i].x > maxX) maxX = coordArray[i].x;
        if (coordArray[i].y > maxY) maxY = coordArray[i].y;
    }

    // 2. Calculate dynamic widths
    width = (maxX - minX) + 1;
    height = (maxY - minY) + 1;

    // 3. Clear our statically allocated arrays
    for (int i = 0; i < (MAX_WIDTH * MAX_HEIGHT); i++) {
        presentMap[i] = false;
        coordToIndex[i] = 255;
    }

    // 4. Populate with re-zeroed coords
    for (uint8_t i = 0; i < totalLEDs; i++) {
        uint8_t adjX = coordArray[i].x - minX;
        uint8_t adjY = coordArray[i].y - minY;

        // Safety check to ensure we don't write outside our static array
        if (adjX >= MAX_WIDTH || adjY >= MAX_HEIGHT) continue;

        // Map to 1D index using MAX_WIDTH so memory lookup is consistent
        uint16_t flatGridIndex = (adjY * MAX_WIDTH) + adjX;

        presentMap[flatGridIndex] = true;
        coordToIndex[flatGridIndex] = i;

        indexToCoord[i].x = adjX;
        indexToCoord[i].y = adjY;
    }
}

bool LedMapper::isPresent(uint8_t x, uint8_t y) const {
    if (x >= width || y >= height) return false;
    return presentMap[(y * MAX_WIDTH) + x];
}

uint8_t LedMapper::getIndex(uint8_t x, uint8_t y) const {
    if (x >= width || y >= height) return 255;
    return coordToIndex[(y * MAX_WIDTH) + x];
}

LedMapper::Coordinate LedMapper::getCoord(uint8_t index) const {
    if (index >= totalLEDs) return {255, 255};
    return indexToCoord[index];
}