#include "Navigation.hpp"

void Navigation::rotateRight(uint8_t speed)
{
   leftWheel.moveForward(speed);
   rightWheel.moveBackward(speed);
}

void Navigation::rotateLeft(uint8_t speed)
{
   leftWheel.moveBackward(speed);
   rightWheel.moveForward(speed);
}

void Navigation::moveForward(uint8_t speed)
{
   leftWheel.moveForward(speed);
   rightWheel.moveForward(speed);
}

void Navigation::moveBackward(uint8_t speed)
{
   leftWheel.moveBackward(speed);
   rightWheel.moveBackward(speed);
}

void Navigation::idle(){
   leftWheel.idle();
   rightWheel.idle();
}