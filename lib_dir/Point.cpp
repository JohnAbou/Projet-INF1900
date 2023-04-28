#include "Point.h"

Point::Point(uint8_t x, uint8_t y, uint8_t directionX, uint8_t directionY)
{
    this->x = x;
    this->y = y;
    this->directionX = directionX;
    this->directionY = directionY;
}

bool Point::isInside(uint8_t nRows, uint8_t nColumns)
{
    return (x >= 0 && x <= nRows && y >= 0 && y <= nColumns);
}

Point Point::tryNext(uint8_t nRows, uint8_t nColumns)
{
    Point temp(x, y, directionX, directionY);
    temp.x += 1 * temp.directionX;
    temp.y += 1 * temp.directionY;

    if (temp.isInside(nRows, nColumns))
    {
        return temp;
    }
    return *this;
}

void Point::updateDirections()
{
    int tempX = directionX;
    int tempY = directionY;
    this->directionX = tempX * 1 + tempY * 1;
    this->directionY = tempX * -1 + tempY * 1;
    this->directionX = directionX == 0 ? 0 : directionX > 0 ? 1 : -1;
    this->directionY = directionY == 0 ? 0 : directionY > 0 ? 1 : -1;
}

void Point::updatePosition(uint8_t distance)
{
    this->x += directionX * distance;
    this->y += directionY * distance;
}