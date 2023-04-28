#include "Interpretor.hpp"

void Interpretor::checkNext()
{
  memory_.lecture(address_++, &operation_);
  memory_.lecture(address_++, &operand_);
}

void Interpretor::executeCase()
{
switch (operation_)
  {
  case ATT:
    for(uint16_t i = 0; i < operand_; i++)
    {
      _delay_ms(25);
    }
    checkNext();
    break;
  case DAL:
    led_.setColor(operand_);
    checkNext();
    break;
  case DET:
    led_.setOff();
    checkNext();
    break;
  case SGO:
    sound_.playNote(operand_);
    checkNext();
    break;
  case SAR:
    sound_.stopSound();
    checkNext();
    break;
  case MAR:
    nav_.idle();
    checkNext();
    break;
  case MAR2:
    nav_.idle();
    checkNext();
    break;
  case MAV:
    nav_.moveForward(operand_);
    checkNext();
    break;
  case MRE:
    nav_.moveBackward(operand_);
    checkNext();
    break;
  case TRD:
    nav_.rotateRight(150);
    _delay_ms(1035);
    nav_.idle();
    _delay_ms(100);
    checkNext();
    break;
  case TRG:
    nav_.rotateLeft(150);
    _delay_ms(1035);
    nav_.idle();
    _delay_ms(100); 
    checkNext();
    break;
  case DBC:
    loopAddress_ = address_;
    loopIncrements_ = operand_;
    loopCounter_ = 0;
    checkNext();
    break;
    case FBC:
    if(loopCounter_ < loopIncrements_)
    {
      address_ = loopAddress_;
      loopCounter_++;
    }
    checkNext();
    break;
    case FIN:
    started_ = false;
    break;
  }
}

void Interpretor::readData()
{
    started_ = false;
    while(!started_)
    {
        checkNext();
        if(operation_ == DBT)
        {
          checkNext();
          started_ = true;
        }
    }

    while(started_)
    {
      executeCase();
    }
}