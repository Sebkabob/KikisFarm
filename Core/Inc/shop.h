
#ifndef SHOP_H_
#define SHOP_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "sprites.h"
#include "game.h"

// Void left
#define OB5_X  0    // Left boundary
#define OB5_Y  46  // Top boundary
#define OB5_W  53   // Width
#define OB5_H  16   // Height

// Void right
#define OB6_X  68   // Left boundary
#define OB6_Y  46  // Top boundary
#define OB6_W  100   // Width
#define OB6_H  16   // Height

void shopPlayerMovement();

void shopDisplay();

void shopPlayerAction();

void handleShop();

#endif /* INC_SHOP_H_ */
