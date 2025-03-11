
#ifndef SHOP_H_
#define SHOP_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "sprites.h"
#include "sound.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include <stdbool.h>
#include <math.h>

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

void textSpeaking(const char *text, int speed, int fontSize, int wait);
int getTillSoilCost(void);
bool shopObstacle(int x, int y);
void shopSoftRefresh(void);
void shopHardRefresh(void);
void shopTextDraw(int itemSelect);
int shopBuyItem(int *money, int level, InventorySlot inventory[], int itemSelect);
bool shopNearBuy(void);
bool shopNearSell(void);
void initShopItems(void);
void shopBuy(void);
void shopSell(void);
void shopPlayerMovement(void);
void shopDisplay(void);
void shopPlayerAction(void);
void handleShop(void);

#endif /* INC_SHOP_H_ */
