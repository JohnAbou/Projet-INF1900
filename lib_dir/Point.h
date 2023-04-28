#pragma once
#include <stdint.h>

class Point
{
public:
    Point(uint8_t x, uint8_t y, uint8_t directionX, uint8_t directionY);
    uint8_t x;
    uint8_t y;
    uint8_t directionX;
    uint8_t directionY;
    Point tryNext(uint8_t nRows, uint8_t nColumns);
    bool isInside(uint8_t nRows, uint8_t nColumns);
    void updateDirections();
    void updatePosition(uint8_t distance);
private:
};