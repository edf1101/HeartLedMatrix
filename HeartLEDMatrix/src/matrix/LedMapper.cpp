/**
 * @file LedMapper.cpp
 * @brief Implementation of the LED mapper coordinate system.
 * @author Ed Fillingham
 * @date April 2026
 */

#include "LedMapper.h"

LedMapper::LedMapper(Coordinate* coordArray, uint8_t count) {
    totalLEDs = count > MAX_LEDS ? MAX_LEDS : count;

    if (totalLEDs == 0) {
        width = 0;
        height = 0;
        return;
    }

    // Find the bounding box of all LED positions
    uint8_t minX = 255, minY = 255;
    uint8_t maxX = 0, maxY = 0;
    for (uint8_t i = 0; i < totalLEDs; i++) {
        if (coordArray[i].x < minX) minX = coordArray[i].x;
        if (coordArray[i].y < minY) minY = coordArray[i].y;
        if (coordArray[i].x > maxX) maxX = coordArray[i].x;
        if (coordArray[i].y > maxY) maxY = coordArray[i].y;
    }

    // Calculate actual grid dimensions
    width = (maxX - minX) + 1;
    height = (maxY - minY) + 1;

    // Clear lookup tables
    for (int i = 0; i < (MAX_WIDTH * MAX_HEIGHT); i++) {
        presentMap[i] = false;
        coordToIndex[i] = 255;
    }

    // Build the mapping tables, translating coordinates to grid origin (0, 0)
    for (uint8_t i = 0; i < totalLEDs; i++) {
        uint8_t adjX = coordArray[i].x - minX;
        uint8_t adjY = coordArray[i].y - minY;

        if (adjX >= MAX_WIDTH || adjY >= MAX_HEIGHT) continue;

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