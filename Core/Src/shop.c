#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "shop.h"
#include "inventory.h"
#include "characters.h"
#include <stdbool.h>
#include <math.h>

#define TOTAL_SHOP_ITEMS 18
#define VISIBLE_SHOP_ITEMS 9

void textSpeaking(const char *text, int speed, int fontSize, int wait);

//------------------------------------------------------------------------------
// Defines obstacles in the shop world.
//------------------------------------------------------------------------------
bool shopObstacle(int x, int y) {
    return (
        (x >= OB5_X && x < OB5_X + OB5_W &&
         y >= OB5_Y && y < OB5_Y + OB5_H) ||

        (x >= OB6_X && x < OB6_X + OB6_W &&
         y >= OB6_Y && y < OB6_Y + OB6_H)
    );
}

//------------------------------------------------------------------------------
// Returns true if the player is near the buy shop.
//------------------------------------------------------------------------------
bool shopNearBuy(){
    int x = player.coordinates.x;
    int y = player.coordinates.y;
    return (
        (x >= 10 && x < 36 &&
         y >= 10 && y < 30)
    );
}

//------------------------------------------------------------------------------
// Returns true if the player is near the sell shop.
//------------------------------------------------------------------------------
bool shopNearSell(){
    int x = player.coordinates.x;
    int y = player.coordinates.y;
    return (
        (x >= 82 && x < 108 &&
         y >= 10 && y < 30)
    );
}

void shopPlayerMovement(void) {
    uint8_t step = 1;  // Fixed movement step

    // UP movement: move the player upward if within bounds
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > 25 && !shopObstacle(player.coordinates.x, nextY)) {
            player.coordinates.y = nextY;
        }
    }

    // DOWN movement: move the player downward if within bounds
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < 64 && !shopObstacle(player.coordinates.x, nextY)) {
            player.coordinates.y = nextY;
        }
    }

    // LEFT movement: move the player left if within bounds
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > 0 && !shopObstacle(nextX, player.coordinates.y)) {
            player.coordinates.x = nextX;
        }
    }

    // RIGHT movement: move the player right if within bounds
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < 117 && !shopObstacle(nextX, player.coordinates.y)) {
            player.coordinates.x = nextX;
        }
    }
}

//------------------------------------------------------------------------------
// Helps get rid of text box quickly before entering store.
//------------------------------------------------------------------------------
void shopHardRefresh(){
    ssd1306_Fill(Black);
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiUpSprite, 9, 11, White);
    ssd1306_UpdateScreen();
}

//------------------------------------------------------------------------------
// Handles everything that happens with buying an item.
//------------------------------------------------------------------------------
int shopBuyItem(int *money, int level, InventorySlot inventory[], int itemSelect) {
    if (itemSelect < 1 || itemSelect > TOTAL_SHOP_ITEMS) {
        return 0;
    }
    Item *selectedItem = &shopItems[itemSelect - 1];
    if (selectedItem->id == TILLSOIL && player.soilSpots >= 16) {
        return 0;
    }
    if (selectedItem->id == HOUSEKEY && game.mileStone >= HOUSE_KEY_BOUGHT) {
        return 0;
    }
    if (selectedItem->id == TILLSOIL) {
        int cost = getTillSoilCost();
        if (*money < cost || level < selectedItem->levelUnlock) {
            return 0;
        }
        *money -= cost;
        player.soilSpots++;
        displayStats();
        ssd1306_UpdateScreen();
        return 1;
    } else {
        if (*money < selectedItem->buyValue || level < selectedItem->levelUnlock) {
            return 0;
        }
    }
    if (selectedItem->id == HOUSEKEY) {
        if (addItemToInventory(inventory, selectedItem, 1)) {
            *money -= selectedItem->buyValue;
            game.mileStone = HOUSE_KEY_BOUGHT;
            displayStats();
            ssd1306_UpdateScreen();
            return 1;
        }
        return 0;
    }
    if (addItemToInventory(inventory, selectedItem, 1)) {
        *money -= selectedItem->buyValue;
        displayStats();
        ssd1306_UpdateScreen();
        return 1;
    }
    return 0; // If inventory is full.
}

//------------------------------------------------------------------------------
// Draws the Price, Title, and Level of an item in the buy store.
//------------------------------------------------------------------------------
void buyShopTextDraw(int itemSelect) {
    if (itemSelect > 0 && itemSelect <= TOTAL_SHOP_ITEMS) {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
        ssd1306_DrawBitmap(57, 2, shopItems[itemSelect - 1].titleSprite, 65, 24, White);
        bool canBuy = false;
        int price = 0;
        if (shopItems[itemSelect - 1].id == TILLSOIL) {
            price = getTillSoilCost();
            canBuy = (player.level >= shopItems[itemSelect - 1].levelUnlock);
            if (player.soilSpots >= 16) {
                canBuy = false;
            }
        } else {
            price = shopItems[itemSelect - 1].buyValue;
            canBuy = (player.level >= shopItems[itemSelect - 1].levelUnlock);
        }
        if (shopItems[itemSelect - 1].id == HOUSEKEY && hasItemInInventory(player.inventory, HOUSEKEY) > 0) {
            canBuy = false;
        }
        if (!canBuy) {
            ssd1306_SetCursor(90 - (7 / 2), 39);
            ssd1306_WriteString("X", Font_7x10, White);
        } else {
            char priceText[20];
            int dollars = price / 100;
            int cents = price % 100;
            if (price > 99999) {
                snprintf(priceText, sizeof(priceText), "$%d", dollars);
            } else {
                snprintf(priceText, sizeof(priceText), "$%d.%02d", dollars, cents);
            }
            int textWidth = strlen(priceText) * 7;
            int centeredX = 90 - (textWidth / 2);
            ssd1306_SetCursor(centeredX, 39);
            ssd1306_WriteString(priceText, Font_7x10, White);
        }
        char levelText[10];
        snprintf(levelText, sizeof(levelText), "Lv. %d", shopItems[itemSelect - 1].levelUnlock);
        int levelWidth = strlen(levelText) * 6;
        int levelX = 91 - (levelWidth / 2);
        ssd1306_SetCursor(levelX, 25);
        ssd1306_WriteString(levelText, Font_6x8, White);
    } else {
        ssd1306_FillRectangle(57, 2, 122, 53, Black);
    }
    ssd1306_DrawRectangle(63, 35, 117, 51, White);
    ssd1306_DrawRectangle(64, 36, 116, 50, White);
}

//------------------------------------------------------------------------------
// Handles the buy shop.
//------------------------------------------------------------------------------
void shopBuy() {
    initShopItems();
    int shopScrollOffset = 0;
    int itemSelect = 1;
    int moved = 1;
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0;
    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
    for (int i = 0; i < VISIBLE_SHOP_ITEMS; i++) {
        int index = shopScrollOffset + i;
        if (index >= TOTAL_SHOP_ITEMS)
            break;
        xMov = ((i % 3) * 17);
        yMov = ((i / 3) * 17);
        ssd1306_DrawBitmap(8 + xMov, 4 + yMov, shopItems[index].itemSprite, 14, 14, White);
    }
    buyShopTextDraw(shopScrollOffset + itemSelect);
    displayStats();
    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;
    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
    if (shopScrollOffset + VISIBLE_SHOP_ITEMS < TOTAL_SHOP_ITEMS)
    	ssd1306_DrawBitmap(58, 51, downArrow, 3, 2, White);
    if (shopScrollOffset > 0)
    	ssd1306_DrawBitmap(58, 5, upArrow, 3, 2, White);
    ssd1306_UpdateScreen();
    while (1) {
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            sound(menuNav);
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
            sound(menuNav);
            moved = 1;
            if (itemSelect <= 3 && shopScrollOffset >= 3) {
                shopScrollOffset -= 3;
            } else if (itemSelect > 3) {
                itemSelect -= 3;
            }
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
            sound(menuNav);
            moved = 1;
            if (itemSelect > 6 && (shopScrollOffset + VISIBLE_SHOP_ITEMS) < TOTAL_SHOP_ITEMS) {
                shopScrollOffset += 3;
                if (shopScrollOffset + VISIBLE_SHOP_ITEMS > TOTAL_SHOP_ITEMS)
                    shopScrollOffset = TOTAL_SHOP_ITEMS - VISIBLE_SHOP_ITEMS;
            } else if (itemSelect <= 6) {
                itemSelect += 3;
                if (shopScrollOffset + itemSelect > TOTAL_SHOP_ITEMS)
                    itemSelect = TOTAL_SHOP_ITEMS - shopScrollOffset;
            }
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0 && (itemSelect % 3) != 1) {
            sound(menuNav);
            moved = 1;
            itemSelect -= 1;
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0 && (itemSelect % 3) != 0) {
            sound(menuNav);
            moved = 1;
            itemSelect += 1;
            if (shopScrollOffset + itemSelect > TOTAL_SHOP_ITEMS)
                itemSelect = TOTAL_SHOP_ITEMS - shopScrollOffset;
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            int actualIndex = shopScrollOffset + itemSelect;
            int didItBuy = shopBuyItem(&player.money, player.level, player.inventory, actualIndex);
            if (didItBuy) {
                sound(buyItem);
            } else {
                sound(cantBuy);
            }
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            moved = 1;
        }
        if (moved) {
            moved = 0;
            ssd1306_FillRectangle(5, 1, 122, 55, Black);
            ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

            for (int i = 0; i < VISIBLE_SHOP_ITEMS; i++) {
                int index = shopScrollOffset + i;
                if (index >= TOTAL_SHOP_ITEMS)
                    break;
                xMov = ((i % 3) * 17);
                yMov = ((i / 3) * 17);
                ssd1306_DrawBitmap(8 + xMov, 4 + yMov, shopItems[index].itemSprite, 14, 14, White);
            }
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;
            buyShopTextDraw(shopScrollOffset + itemSelect);
            displayStats();
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
            if (shopScrollOffset + VISIBLE_SHOP_ITEMS < TOTAL_SHOP_ITEMS)
            	ssd1306_DrawBitmap(58, 51, downArrow, 3, 2, White);
            if (shopScrollOffset > 0)
            	ssd1306_DrawBitmap(58, 5, upArrow, 3, 2, White);
            ssd1306_UpdateScreen();
        }
    }
}

//------------------------------------------------------------------------------
// Draws the Price, Title, and Level of an item in the buy store.
//------------------------------------------------------------------------------
void drawSellValue(int itemSelect){
    int sellPrice;
    char priceText[20];
    if(player.inventory[itemSelect - 1].quantity > 0) {
        sellPrice = player.inventory[itemSelect - 1].item->sellValue * player.inventory[itemSelect - 1].quantity;
        int dollars = sellPrice / 100;
        int cents = sellPrice % 100;
        if(sellPrice > 9999) {
            snprintf(priceText, sizeof(priceText), "+$%d", dollars);
        } else {
            snprintf(priceText, sizeof(priceText), "+$%d.%02d", dollars, cents);
        }
        int textWidth = strlen(priceText) * 7;
        int centeredX = 90 - (textWidth / 2);
        ssd1306_SetCursor(centeredX, 39);
        ssd1306_WriteString(priceText, Font_7x10, White);
    } else {
        ssd1306_SetCursor(90 - (7 / 2), 39);
        ssd1306_WriteString("X", Font_7x10, White);
    }
    ssd1306_DrawRectangle(63, 35, 117, 51, White);
    ssd1306_DrawRectangle(64, 36, 116, 50, White);
}

//------------------------------------------------------------------------------
// Handles the sell shop.
//------------------------------------------------------------------------------
void shopSell(){
    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
    int itemSelect = 1;
    int moved = 1;
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0;
    drawInventoryIcons(0, 0);
    drawItemInfo(itemSelect);
    drawSellValue(itemSelect);
    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;
    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
    ssd1306_UpdateScreen();
    while (1) {
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0 && itemSelect > 3) {
            moved = 1;
            sound(menuNav);
            itemSelect -= 3;
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0 && itemSelect < 7) {
            moved = 1;
            sound(menuNav);
            itemSelect += 3;
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0 && (itemSelect % 3) != 1) {
            moved = 1;
            sound(menuNav);
            itemSelect -= 1;
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0 && (itemSelect % 3) != 0) {
            moved = 1;
            sound(menuNav);
            itemSelect += 1;
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }
        uint32_t holdTime = 500;
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            uint32_t pressStart = HAL_GetTick();
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
                if (HAL_GetTick() - pressStart >= holdTime) {
                    break;
                }
            }
            uint32_t pressDuration = HAL_GetTick() - pressStart;
            InventorySlot *slot = &player.inventory[itemSelect - 1];
            if (slot->item != NULL && slot->item->id != NONE && slot->quantity > 0) {
                if (pressDuration >= holdTime) {
                    int quantity = slot->quantity;
                    int totalSellPrice = slot->item->sellValue * quantity;
                    removeItemFromInventory(player.inventory, slot->item->id, quantity);
                    player.money += totalSellPrice;
                } else {
                    int sellPrice = slot->item->sellValue;
                    removeItemFromInventory(player.inventory, slot->item->id, 1);
                    player.money += sellPrice;
                }
                sound(sellItem);
                displayStats();
                moved = 1;
            } else {
            	sound(cantSell);
            }
        }
        if (moved) {
            moved = 0;
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;
            ssd1306_FillRectangle(5, 1, 122, 55, Black);
            ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
            drawInventoryIcons(0, 0);
            drawItemInfo(itemSelect);
            drawSellValue(itemSelect);
            displayStats();
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
            ssd1306_UpdateScreen();
        }
    }
}

//------------------------------------------------------------------------------
// Draws the background elements.
//------------------------------------------------------------------------------
void shopDisplay(){
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
}

//------------------------------------------------------------------------------
// Handles button actions.
//------------------------------------------------------------------------------
void shopPlayerAction(){
    if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearBuy()) {
    	shopHardRefresh();
        textSpeaking("Goods at a good     price! (kind of)", 400, 9, 1);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        shopHardRefresh();
        shopBuy();
    } else if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearSell()){
    	if (!isInventoryEmpty(player.inventory)){
            textSpeaking("I'll buy stuff!     (I will lowball you)", 110, 7, 1);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            shopHardRefresh();
            shopSell();
    	} else {
    		textSpeaking("Come back with stuff to sell!", 110, 7, 1);
    		while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    	}
    } else if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
    	petFeed();
    }
    if (B_Button_Flag) {
    	uint32_t startTime = HAL_GetTick();
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
            if (HAL_GetTick() - startTime >= 1500) {
            	petSit();
                while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
                return;
            }
        }
        showInventory(0);
    }
    if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1 && game.mileStone >= MAP_ACQUIRED) {
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        theMap();
    }
    if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) { //menu mode
    	SELECT_Button_Flag = 0;
        while(HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
        statbarShow = 0;
        int p = gameMenu();
        if(p) leaveWorld = 1;
    }
}

//------------------------------------------------------------------------------
// Main loop for the shop world: updates movement, actions, and the full display.
//------------------------------------------------------------------------------
void handleShop() {
    ssd1306_Fill(Black);
    shopDisplay();
    catDisplay();
	playerDisplay();
	ssd1306_UpdateScreen();

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;  // Update every 20 ms

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {

        	ssd1306_Fill(Black);

        	shopDisplay();

        	updateButtonFlags();
            shopPlayerMovement();

            catDisplay();
        	playerDisplay();

            shopPlayerAction();

        	ssd1306_UpdateScreen();
            lastFrameTime = now;
        }

        gameLogic();
        HAL_Delay(1);

        if (player.coordinates.y >= 62) {
            player.inWorld = CROP;
            player.coordinates.x = 60;
            player.coordinates.y = 10;
            player.direction = DOWN;
            break;
        }
    }
}
