#ifndef SOUND_H_
#define SOUND_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

// Octave 4 (approximate frequencies in Hz)
#define C4   261
#define C4s  277
#define D4   293
#define D4s  311
#define E4   329
#define F4   349
#define F4s  370
#define G4   392
#define G4s  415
#define A4   440
#define A4s  466
#define B4   494

// Octave 5
#define C5   523
#define C5s  554
#define D5   587
#define D5s  622
#define E5   659
#define F5   698
#define F5s  740
#define G5   784
#define G5s  830
#define A5   880
#define A5s  932
#define B5   988

extern const uint16_t menuOpen[][2];
extern const uint16_t menuNav[][2];
extern const uint16_t menuSelect[][2];
extern const uint16_t menuExit[][2];
extern const uint16_t savingGame[][2];
extern const uint16_t gameSaved[][2];
extern const uint16_t sellItem[][2];
extern const uint16_t buyItem[][2];
extern const uint16_t cantBuy[][2];
extern const uint16_t cantSell[][2];
extern const uint16_t areYouSure[][2];
extern const uint16_t deleteSave[][2];
extern const uint16_t buttonPress[][2];
extern const uint16_t playGame[][2];
extern const uint16_t levelUp[][2];
extern const uint16_t harvest[][2];
extern const uint16_t destroy[][2];
extern const uint16_t inventoryOpen[][2];
extern const uint16_t inventoryFull[][2];
extern const uint16_t mapOpen[][2];
extern const uint16_t catMeow[][2];
extern const uint16_t catFeed[][2];
extern const uint16_t catSit[][2];
extern const uint16_t catStand[][2];

void sound(const uint16_t soundArray[][2]);


#endif /* INC_SOUND_H_ */
