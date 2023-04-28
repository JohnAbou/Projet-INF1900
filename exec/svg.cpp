/*
Reference pour la formule du calcul d'aire : https://www.mathopenref.com/coordpolygonarea.html
Utilisation de la marche de Jarvis pour le calcul de l'enveloppe : https://fr.wikipedia.org/wiki/Marche_de_Jarvis
*/
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can.h"
#include "can.cpp"
#include "Led.hpp"
#include "Point.h"
#include "Communication.hpp"
#include "memoire_24.h"
#include "Navigation.hpp"
#include "Timer.hpp"
#include "Sound.hpp"
#include "Button.hpp"
#include <stdio.h>
#include <math.h>
#include <string.h>


//#include "memory_24.h"


uint16_t adress = 0;
const uint16_t nPoteaux = 6;
uint16_t listeX[nPoteaux] = {412, 522, 302, 302, 632, 302};
uint16_t listeY[nPoteaux] = {233, 123, 233, 343, 233, 123};
uint16_t enveloppeX[nPoteaux];
uint16_t enveloppeY[nPoteaux];
uint16_t nouvelleEnveloppeX[nPoteaux];
uint16_t nouvelleEnveloppeY[nPoteaux];
uint16_t listeATrier[nPoteaux * 2];
uint16_t nouvelleTaille = 0;
Communication comm;

uint32_t crc_global = 0xFFFFFFFF;

uint32_t calculate_crc32(const unsigned char *data, size_t length, uint32_t initial_crc) {
uint32_t crc = initial_crc;
uint32_t polynomial = 0xEDB88320;

for (size_t i = 0; i < length; i++)
 {
    uint8_t byte = data[i];
    crc ^= byte;
    for (size_t j = 0; j < 8; j++) 
    {
        uint32_t mask = -(crc & 1);
        crc = (crc >> 1) ^ (polynomial & mask);
        }
}

return crc;
}

void update_crc32(const char *str)
{
size_t str_length = custom_strlen(str);
crc_global = calculate_crc32((unsigned char*)str, str_length, crc_global);
}
//0x104C11DB7
/*uint32_t crc(uint bitString[32], uint8_t polynome)
{
    uint8_t shiftRegister = valeur initiale;  // Généralement tous les bits à 0 ou 1
    for(uint8_t i = 0; i < 32; i++)
    {
        if((valeur initiale << bit de poids moins fort ^ bitString[i]) & 1)
        {
            shiftRegister = (shiftRegister << 1) ^ polynome;
        }
        else
        {
            shiftRegister = (shiftRegister  << 1);

        }
    }
    return shiftRegister;
}*/
/* void lectureMemoire()
{
 for (int i = 0; i < nPoteaux; i++)
    {
        memory.lecture(adress++, &x);
        memory.lecture(adress++, &y);
        x = (x * 110) + 192;
        listeX[i] = x;
        y = (y * 110) + 123;
        listeY[i] = y;
    }
} */


uint16_t orientation(uint16_t p, uint16_t q, uint16_t r)
{
    uint16_t val = (listeY[q] - listeY[p]) * (listeX[r] - listeX[q]) - (listeX[q] - listeX[p]) * (listeY[r] - listeY[q]);
    if (val == 0)
    {
        return 0;
    }
    return (val > 0) ? 1:2;
}

void enveloppeConvexe()
{
    uint16_t enveloppe[nPoteaux];
    for (uint16_t i = 0; i < nPoteaux; i++)
    {
         enveloppe[i] = 0;
    }

    uint16_t l = 0;
    for (uint16_t i = 1; i < nPoteaux; i++)
        if (listeX[i] < listeX[l])
            l = i;

    uint16_t p = l, q;
    do
    {
        enveloppe[p] = 1; 

        q = (p + 1) % nPoteaux;
        for (uint16_t r = 0; r < nPoteaux; r++)
            if (orientation(p, q, r) == 2)
                q = r;

        p = q; 
    } while (p != l); 

    uint16_t taille = 0;
    for (uint16_t i = 0; i < sizeof(enveloppe)/sizeof(enveloppe[0]); i++)
    {
        if (enveloppe[i] == 1){
            enveloppeX[taille] = listeX[i];
            enveloppeY[taille] = listeY[i];
            taille++;
        }
    }
    
    for (uint16_t i = 0; i < taille; i++)
    {
        bool contained = false;
        for (uint16_t j = 0; j < nouvelleTaille; j++)
        {
            if (enveloppeX[i] == nouvelleEnveloppeX[j] && enveloppeY[i] == nouvelleEnveloppeY[j])
            {
                contained = true;
                break;
            }
        }

        if (!contained) {
            nouvelleEnveloppeX[nouvelleTaille] = enveloppeX[i];
            nouvelleEnveloppeY[nouvelleTaille] = enveloppeY[i];
            nouvelleTaille++;
        }
    }
   
    uint16_t compteur = 0;

    for (uint16_t i = 0; i < nouvelleTaille; i++)
    {
        listeATrier[compteur] = nouvelleEnveloppeX[i];
        listeATrier[compteur + 1] = nouvelleEnveloppeY[i];
        compteur += 2;
    }
   /*for (uint8_t i = 0; i < nouvelleTaille; i++)
   {
   comm.transmissionUART(nouvelleEnveloppeX[i]);
    _delay_ms(5);
    }*/

  /*  char buffer[100];
    sprintf(buffer, "<text x=\"96\" y=\"564\" font-family=\"arial\" font-size=\"20\" fill=\"blue\">AIRE: \"%d\" pouces carrees</text>\n", nouvelleTaille);
    for (size_t i = 0; i < strlen(buffer); i++)
    {
        comm.transmissionUART(buffer[i]);
        _delay_ms(5);
    }*/
   
}

void triageListe()
{

    uint16_t x = 0, y = 0;
    for (uint16_t i = 0; i < nouvelleTaille * 2; i += 2)
    {
        x += listeATrier[i];
        y += listeATrier[i + 1];
    }
    x /= nouvelleTaille;
    y /= nouvelleTaille;

    for (uint16_t i = 0; i < nouvelleTaille * 2; i += 2)
    {
        uint16_t angle1 = atan2(listeATrier[i + 1] - y, listeATrier[i] - x);
        for (uint16_t j = i + 2; j < nouvelleTaille * 2; j += 2)
        {
            uint16_t angle2 = atan2(listeATrier[j + 1] - y, listeATrier[j] - x);
            if (angle1 > angle2)
            {
         
                uint16_t temp = listeATrier[i];
                listeATrier[i] = listeATrier[j];
                listeATrier[j] = temp;
                temp = listeATrier[i + 1];
                listeATrier[i + 1] = listeATrier[j + 1];
                listeATrier[j + 1] = temp;
                angle1 = angle2;
            }
        }
    }
    uint16_t compteur = 0;
    for (uint16_t i = 0; i < nouvelleTaille; i++)
    {
        nouvelleEnveloppeX[i] = listeATrier[compteur];
        nouvelleEnveloppeY[i] = listeATrier[compteur + 1];
        compteur += 2;
    }

}

uint16_t airPrisme(uint16_t nouvelleTaille, uint16_t listeFinaleX[], uint16_t listeFinaleY[])
{
    uint16_t aire = 0.0;
    
    for (uint16_t i = 0; i < nouvelleTaille; i++) 
    {
        uint16_t index = (i + 1) % nouvelleTaille; 
        aire += listeFinaleX[i] * listeFinaleY[index] - listeFinaleY[i] * listeFinaleX[index];
    }

    if(aire < 0)
    {
        aire *= -1;
    }

    return (aire/2.0) / 100;
}


int main()
{
    comm.initUART();
    comm.transmissionUART(0x02);

    enveloppeConvexe();
    triageListe();
    char buffer[480];
    char buffer2[80];
    uint16_t valeurX;
    uint16_t valeurY;
    char fin[] = "</svg>";

    
    char textDebut[] = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"100%\" viewBox=\"0 0 1152 576\">\n"
           "<text x=\"76\" y=\"36\" font-family=\"arial\" font-size=\"25\" fill=\"blue\"> Section 05 -- Equipe 3536 -- Bin2Dec</text>\n"
           "<rect x=\"96\" y=\"48\" width=\"960\" height=\"480\" stroke=\"black\" stroke-width=\"1\" fill=\"white\"/>\n";

    size_t textDebut_length = strlen(textDebut);
    for (size_t i = 0; i < textDebut_length; i++)
    {
        _delay_ms(5);
        comm.transmissionUART(textDebut[i]);
    }


    sprintf(buffer, "<text x=\"96\" y=\"564\" font-family=\"arial\" font-size=\"20\" fill=\"blue\">AIRE: \"%d\" pouces carrees</text>\n", airPrisme(nouvelleTaille, nouvelleEnveloppeX, nouvelleEnveloppeY));
    for (size_t i = 0; i < strlen(buffer); i++)
    {
        comm.transmissionUART(buffer[i]);
        _delay_ms(5);
    }
   

    
    for(uint16_t i = 0; i < nouvelleTaille; i++)
    {
        char point[50];
        sprintf(point, "%d,%d ", nouvelleEnveloppeX[i], nouvelleEnveloppeY[i]);
        strcat(buffer2, point);
    }
    sprintf(buffer, "<polygon points=\"%s\" style=\"fill:green;stroke:black;stroke-width:3\" />\n", buffer2);
    for (size_t i = 0; i < strlen(buffer); i++)
    {
        comm.transmissionUART(buffer[i]);
        _delay_ms(5);
    }


    for (uint16_t i = 0; i < 8; i++)
    {
        valeurX = i * 110 + 191;
        for (uint16_t j = 0; j < 4; j++)
        {
            valeurY = j * 110 + 123;
            sprintf(buffer, "<rect x=\"%d\" y=\"%d\" width=\"5\" height=\"5\"/>\n", valeurX, valeurY);
            for (size_t index = 0; index < strlen(buffer); index++)
            {
                comm.transmissionUART(buffer[index]);
                _delay_ms(5);
            }
        }
    }

  char rectangleRouge[] = "<rect x=\"191\" y=\"453\" width=\"5\" height=\"5\" fill=\"red\"/>\n";
     for (size_t i = 0; i < strlen(rectangleRouge); i++)
     {
         comm.transmissionUART(rectangleRouge[i]);
         _delay_ms(5);
     }

    for (uint16_t i = 0; i < nPoteaux; i++)
    {
        sprintf(buffer, "<circle cx=\"%d\" cy=\"%d\" r=\"10\" stroke=\"black\" stroke-width=\"4\" fill=\"grey\" />\n", listeX[i], listeY[i]);
        for (size_t i = 0; i < strlen(buffer); i++)
        {
            comm.transmissionUART(buffer[i]);
            _delay_ms(5);
        }
    }

    for (uint16_t i = 0; i < 6; i++)
        {
            comm.transmissionUART(fin[i]);
            _delay_ms(5);
        }


    comm.transmissionUART(0x03);
    _delay_ms(5);
    comm.transmissionUART(0x04);
}