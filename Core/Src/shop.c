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
// .
//------------------------------------------------------------------------------
void shopSoftRefresh(){
    ssd1306_Fill(Black);
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
    ssd1306_UpdateScreen();
}

void shopHardRefresh(){
    ssd1306_Fill(Black);
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
    ssd1306_UpdateScreen();
}

void shopTextDraw(int itemSelect) {

    if (itemSelect > 0 && itemSelect <= TOTAL_SHOP_ITEMS) {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
        ssd1306_DrawBitmap(57, 2, shopItems[itemSelect - 1].titleSprite, 65, 24, White);

        bool canBuy = false;
        int price = 0;
        // For TillSoil, use dynamic cost and only check level (plus a limit on soilSpots)
        if (shopItems[itemSelect - 1].id == TILLSOIL) {
            price = getTillSoilCost();
            canBuy = (player.level >= shopItems[itemSelect - 1].levelUnlock);
            if (player.soilSpots >= 16) {
                canBuy = false; // Cannot buy more than 15 TillSoil items.
            }
        } else {
            price = shopItems[itemSelect - 1].buyValue;
            // Only check if the player's level is high enough
            canBuy = (player.level >= shopItems[itemSelect - 1].levelUnlock);
        }

        // Additionally, if the item is HouseKey and the player already owns one, disallow purchase.
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
            // If price is greater than 99999, drop the decimal place and only show dollars
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


int shopBuyItem(int *money, int level, InventorySlot inventory[], int itemSelect) {
    if (itemSelect < 1 || itemSelect > TOTAL_SHOP_ITEMS) {
        return 0; // Invalid selection.
    }

    Item *selectedItem = &shopItems[itemSelect - 1];

    // Check if the item is TillSoil and if the player has reached the maximum.
    if (selectedItem->id == TILLSOIL && player.soilSpots >= 16) {
        return 0;
    }

    // Check if HouseKey is already owned.
    if (selectedItem->id == HOUSEKEY && game.mileStone >= HOUSE_KEY_BOUGHT) {
        return 0;
    }

    // Check money and level requirements. For TillSoil, use the dynamic cost.
    if (selectedItem->id == TILLSOIL) {
        int cost = getTillSoilCost();
        if (*money < cost || level < selectedItem->levelUnlock) {
            return 0;
        }
        *money -= cost;
        player.soilSpots++;  // Increase soil spots.
        displayStats();
        ssd1306_UpdateScreen();
        return 1;
    } else {
        if (*money < selectedItem->buyValue || level < selectedItem->levelUnlock) {
            return 0;
        }
    }

    // Handle HouseKey as a special case.
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

    // For other items, add them to inventory.
    if (addItemToInventory(inventory, selectedItem, 1)) {
        *money -= selectedItem->buyValue;
        displayStats();
        ssd1306_UpdateScreen();
        return 1;
    }

    return 0; // If inventory is full.
}


bool shopNearBuy(){
    int x = player.coordinates.x;
    int y = player.coordinates.y;
    return (
        (x >= 10 && x < 36 &&
         y >= 10 && y < 30)
    );
}

bool shopNearSell(){
    int x = player.coordinates.x;
    int y = player.coordinates.y;
    return (
        (x >= 82 && x < 108 &&
         y >= 10 && y < 30)
    );
}


void shopBuy() {
    initShopItems();

    // Reset scroll offset for each shop session.
    int shopScrollOffset = 0;

    // itemSelect is 1-indexed (values 1 to VISIBLE_SHOP_ITEMS)
    int itemSelect = 1;
    int moved = 1;
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0;

    // Draw the initial background and UI.
    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

    // Draw visible shop items based on the current scroll offset.
    for (int i = 0; i < VISIBLE_SHOP_ITEMS; i++) {
        int index = shopScrollOffset + i;
        if (index >= TOTAL_SHOP_ITEMS)
            break;
        xMov = ((i % 3) * 17);
        yMov = ((i / 3) * 17);
        ssd1306_DrawBitmap(8 + xMov, 4 + yMov, shopItems[index].itemSprite, 14, 14, White);
    }

    // Display the text/info for the currently selected item.
    shopTextDraw(shopScrollOffset + itemSelect);
    displayStats();

    // Draw the initial selection rectangle.
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
        // --- PRIORITIZE EXIT ---
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            sound(menuNav);
            // Wait for B button release.
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;  // Exit shopBuy()
        }

        // --- NAVIGATION INPUTS ---
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

        // --- REDRAW UI IF NEEDED ---
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

            // Erase previous selection rectangle.
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);

            // Recalculate new selection rectangle.
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;

            shopTextDraw(shopScrollOffset + itemSelect);
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


void drawSellValue(int itemSelect){
    int sellPrice;
    char priceText[20];
    // If the player owns at least one of the item, calculate the total sell price.
    if(player.inventory[itemSelect - 1].quantity > 0) {
        sellPrice = player.inventory[itemSelect - 1].item->sellValue * player.inventory[itemSelect - 1].quantity;
        int dollars = sellPrice / 100;
        int cents = sellPrice % 100;
        // If sellPrice is above 9999, show only the dollar amount.
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




void shopSell(){
    // Clear the screen and draw the board background
    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

    int itemSelect = 1;  // initial selection index (1-9)
    int moved = 1;       // flag to indicate redraw is needed
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0;

    // Draw the inventory items and info.
    drawInventoryIcons(0, 0);
    drawItemInfo(itemSelect);
    drawSellValue(itemSelect);

    // Draw the initial selection box.
    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;
    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
    ssd1306_UpdateScreen();

    while (1) {
        // Navigation: move the selection using the directional buttons
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

        // B button to exit the selling UI.
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }

        // A button to sell the currently selected item.
        uint32_t holdTime = 500;  // Change this value as needed.
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            uint32_t pressStart = HAL_GetTick();
            // Wait until the button is released or holdTime has passed.
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
                if (HAL_GetTick() - pressStart >= holdTime) {
                    break;
                }
            }
            uint32_t pressDuration = HAL_GetTick() - pressStart;

            InventorySlot *slot = &player.inventory[itemSelect - 1];
            // Check if the slot contains a valid item.
            if (slot->item != NULL && slot->item->id != NONE && slot->quantity > 0) {
                if (pressDuration >= holdTime) {
                    // If held for at least holdTime ms, sell all items in the slot.
                    int quantity = slot->quantity;
                    int totalSellPrice = slot->item->sellValue * quantity;
                    removeItemFromInventory(player.inventory, slot->item->id, quantity);
                    player.money += totalSellPrice;
                } else {
                    // Otherwise, sell one unit.
                    int sellPrice = slot->item->sellValue;
                    removeItemFromInventory(player.inventory, slot->item->id, 1);
                    player.money += sellPrice;
                }
                sound(sellItem);
                displayStats();  // update money display if needed
                moved = 1;
            } else {
                // No valid item selected: provide an error beep.
            	sound(cantSell);
            }
        }


        if (moved) {
            moved = 0;
            // Erase previous selection box.
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);

            // Update the selection box coordinates.
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;

            // Redraw the background and inventory UI.
            ssd1306_FillRectangle(5, 1, 122, 55, Black);
            ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
            drawInventoryIcons(0, 0);
            drawItemInfo(itemSelect);
            drawSellValue(itemSelect);
            displayStats();  // update money display if needed

            // Draw the new selection box.
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
            ssd1306_UpdateScreen();
        }
    }
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

void shopDisplay(){
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
}

void shopPlayerAction(){
    if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearBuy()) {
        shopSoftRefresh();
        textSpeaking("Goods at a good     price! (kind of)", 400, 9, 1);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        shopHardRefresh();
        shopBuy();
    } else if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearSell()){
    	if (!isInventoryEmpty(player.inventory)){
            textSpeaking("I'll buy stuff!     (I will lowball you)", 110, 7, 1);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            shopSoftRefresh();
            shopSell();
            shopHardRefresh();
    	} else {
    		textSpeaking("Come back with stuff to sell!", 110, 7, 1);
    		while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    	}
    } else if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
    	petFeed();
    }

    if (B_Button_Flag) {
    	while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
    	B_Button_Flag = 0;
        showInventory(0);
    }

    if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1 && game.mileStone >= MAP_ACQUIRED) {
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        theMap();
    }

    if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) { //menu mode
    	SELECT_Button_Flag = 0;
        while(HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
        shopSoftRefresh();
        statbarShow = 0;
        int p = gameMenu();
        shopHardRefresh();
        if(p) leaveWorld = 1;
    }
}

void handleShop() {
    // Set initial shop state (starting position and direction)
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

        // Exit condition: if player goes across bridge
        if (player.coordinates.y >= 62) {
            player.inWorld = CROP;
            player.coordinates.x = 60;
            player.coordinates.y = 10;
            player.direction = DOWN;
            break;
        }
    }
}
