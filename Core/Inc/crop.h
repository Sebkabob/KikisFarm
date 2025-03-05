
#ifndef CROP_H_
#define CROP_H_

#include "game.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "sprites.h"
#include <stdbool.h>

extern uint32_t cropPlantTimes[10]; // Declare globally so other files can use it

// House
#define OB1_X  0   // Left boundary
#define OB1_Y  0   // Top boundary
#define OB1_W  26  // Width
#define OB1_H  15  // Height

// Tree
#define OB2_X  118  // Left boundary
#define OB2_Y  0    // Top boundary
#define OB2_W  30   // Width
#define OB2_H  18   // Height

// Void left
#define OB3_X  0    // Left boundary
#define OB3_Y  -12  // Top boundary
#define OB3_W  54   // Width
#define OB3_H  16   // Height

// Void right
#define OB4_X  66   // Left boundary
#define OB4_Y  -12  // Top boundary
#define OB4_W  100   // Width
#define OB4_H  16   // Height

#define cropSpotXc1 3
#define cropSpotXc2 29
#define cropSpotXc3 55
#define cropSpotXc4 81
#define cropSpotXc5 107

#define cropSpotYr1	24
#define cropSpotYr2 39


bool cropObstacle(int x, int y);
void cropPlayerMovement();
void cropPlayerAction();
void gameLogic(void);
int showInventory(int plantSeed);
int updateBatteryLife(void);
void gameOptions(void);

void updateButtonFlags();
void playerDisplay();


#if defined(XBM)
void cropDisplay(u8g2_t *display);
#endif

#if defined(CPP)
void cropDisplay();
#endif

void handleCrop();

#endif /* INC_CROP_H_ */
