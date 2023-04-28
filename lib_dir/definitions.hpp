/*
////////////////////////////////////////////////////////////////////////////////
/// \file    definitions.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
             Jérôme Fréchette
             John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Définition de types et de constantes utiles pour travailler avec des registres.

Identifications matérielles (Broches I/O): Aucun matériel utilisé pour ce fichier
*/

#pragma once
#include <stdint.h>
#include <avr/io.h>

typedef volatile uint8_t* register_t;
const uint8_t INPUT_MODE = 0x00;
const uint8_t OUTPUT_MODE = 0XFF;
const uint8_t LED_OFF = 0;
const uint8_t LED_GREEN = (1 << PA1);
const uint8_t LED_RED = (1 << PA0);