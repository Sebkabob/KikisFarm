#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "shop.h"
#include <stdbool.h>
#include <math.h>


const unsigned char *itemTitles[] = {
    WheatSeedsTitle,
    CornSeedsTitle,
    PotatoSeedsTitle,
    CarrotSeedsTitle,
    PumpkinSeedsTitle,
    SugarSeedsTitle,
    TillMoreSoilTitle,
    HouseKeyTitle
};

void textSpeaking(const char *text, int speed, int fontSize, int wait);

int getTillSoilCost() {
    int baseCost = 100;
    double multiplier = 1.35; // 35% increase per spot owned
    return (int)(baseCost * pow(multiplier, player.soilSpots));
}

bool shopObstacle(int x, int y) {
    return (
        (x >= OB5_X && x < OB5_X + OB5_W &&
         y >= OB5_Y && y < OB5_Y + OB5_H) ||

        (x >= OB6_X && x < OB6_X + OB6_W &&
         y >= OB6_Y && y < OB6_Y + OB6_H)
    );
}

void shopSoftRefresh(){
    ssd1306_Fill(Black);        // Clear the screen
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
    ssd1306_UpdateScreen();
}

void shopHardRefresh(){
    ssd1306_Fill(Black);        // Clear the screen
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
    ssd1306_UpdateScreen();
}

void shopTextDraw(int itemSelect) {
    if (itemSelect > 0 && itemSelect <= 8) {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
        ssd1306_DrawBitmap(57, 2, itemTitles[itemSelect - 1], 65, 24, White);

        bool canBuy = false;
        int price = 0;
        // For TillSoil, use dynamic cost and only check level (plus a limit on soilSpots)
        if (shopItems[itemSelect - 1].id == TILLSOIL) {
            price = getTillSoilCost();
            canBuy = (player.level >= shopItems[itemSelect - 1].levelUnlock);
            if (player.soilSpots >= 10) {
                canBuy = false; // Cannot buy more than 9 TillSoil items.
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
            ssd1306_SetCursor(91 - (7 / 2), 39);
            ssd1306_WriteString("X", Font_7x10, White);
        } else {
            char priceText[10];
            snprintf(priceText, sizeof(priceText), "$%d", price);
            int textWidth = strlen(priceText) * 7;
            int centeredX = 91 - (textWidth / 2);
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

    ssd1306_DrawRectangle(66, 35, 115, 51, White);
    ssd1306_DrawRectangle(67, 36, 114, 50, White);
}



int shopBuyItem(int *money, int level, InventorySlot inventory[], int itemSelect) {
    if (itemSelect < 1 || itemSelect > 8) {
        return 0; // Invalid selection.
    }

    Item *selectedItem = &shopItems[itemSelect - 1];

    // Check if the item is TillSoil and if the player has reached the maximum.
    if (selectedItem->id == TILLSOIL && player.soilSpots >= 10) {
        return 0;
    }

    // Check if HouseKey is already owned.
    if (selectedItem->id == HOUSEKEY && hasItemInInventory(inventory, HOUSEKEY) > 0) {
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

    int itemSelect = 1;
    int moved = 1;
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0;

    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

    for (int i = 0; i < 8; i++) {
        xMov = ((i % 3) * 17);
        yMov = ((i / 3) * 17);
        ssd1306_DrawBitmap(8 + xMov, 4 + yMov, shopItems[i].seedSprite, 14, 14, White);
    }

    shopTextDraw(itemSelect);
    displayStats();

    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;

    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
    ssd1306_UpdateScreen();

    while (1) {
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0 && itemSelect > 3) {
        	buzzer(300, 15);
            moved = 1;
            itemSelect -= 3;
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0 && itemSelect < 7) {
        	buzzer(300, 15);
            moved = 1;
            itemSelect += 3;
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0 && (itemSelect % 3) != 1) {
        	buzzer(300, 15);
            moved = 1;
            itemSelect -= 1;
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0 && (itemSelect % 3) != 0) {
        	buzzer(300, 15);
            moved = 1;
            itemSelect += 1;
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }

        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
        	buzzer(200, 15);
            shopHardRefresh();
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }

        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            int didItBuy = shopBuyItem(&player.money, player.level, player.inventory, itemSelect);
            if (didItBuy){
            	buzzer(440, 15);
            	buzzer(540, 15);
            }
            else {
            	buzzer(300, 30);
            	buzzer(200, 30);
            }
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            moved = 1;
        }

        if (moved) {
            moved = 0;
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;
            shopTextDraw(itemSelect);
            displayStats();
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
            ssd1306_UpdateScreen();
        }
    }
}

void shopSell(){
    // Implement selling logic here
}

void shopPlayerMovement() {
    static uint8_t downCounter = 0, upCounter = 0, leftCounter = 0, rightCounter = 0;
    uint8_t step = 1;  // Fixed movement step

    if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
        downCounter++;
        refresh++;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < 64 && !shopObstacle(player.coordinates.x, nextY)) {
            player.coordinates.y = nextY;
        }
    } else {
        downCounter = 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
        upCounter++;
        refresh++;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > 25 && !shopObstacle(player.coordinates.x, nextY)) {
            player.coordinates.y = nextY;
        }
    } else {
        upCounter = 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
        leftCounter++;
        refresh++;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > 0 && !shopObstacle(nextX, player.coordinates.y)) {
            player.coordinates.x = nextX;
        }
    } else {
        leftCounter = 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
        rightCounter++;
        refresh++;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < 117 && !shopObstacle(nextX, player.coordinates.y)) {
            player.coordinates.x = nextX;
        }
    } else {
        rightCounter = 0;
    }
}

void shopDisplay(){
    char coords[16];   // Buffer to hold the formatted coordinates string
    sprintf(coords, "(%d, %d)", player.coordinates.x, player.coordinates.y);

    ssd1306_FillRectangle(0, 54, 128, 64, Black);
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_FillRectangle(player.coordinates.x + 2, player.coordinates.y + 2,
            player.coordinates.x + 5, player.coordinates.y + 5, Black);
    if(statbarShow) displayStats();

    if(refresh){
        refresh = 0;
        switch(player.direction) {
        case DOWN:
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
            ssd1306_UpdateScreen();
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, Black);
            break;
        case UP:
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiUpSprite, 9, 11, White);
            ssd1306_UpdateScreen();
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiUpSprite, 9, 11, Black);
            break;
        case LEFT:
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiLeftSprite, 9, 11, White);
            ssd1306_UpdateScreen();
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiLeftSprite, 9, 11, Black);
            break;
        case RIGHT:
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiRightSprite, 9, 11, White);
            ssd1306_UpdateScreen();
            ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiRightSprite, 9, 11, Black);
            break;
        default:
            break;
        }
    }
}

void shopPlayerAction(){
    if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearBuy()) {
        shopSoftRefresh();
        textSpeaking("welcome!            you aren't going to rob me are you?", 150, 8, 1);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        shopHardRefresh();
        shopBuy();
    } else if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && shopNearSell()){
        shopSoftRefresh();
        textSpeaking("not open! uh..      please go away...", 500, 7, 1);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        shopHardRefresh();
    }

    if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
        buzzer(300,25);
        while(HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
        statbarShow = !statbarShow;
        refresh = 1;
    }

    if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        shopSoftRefresh();
        showInventory(0);
        shopHardRefresh();
    }

    if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) { //menu mode
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
    player.coordinates.x = 58;
    player.coordinates.y = 48;
    player.direction = UP;

    ssd1306_Fill(Black);  // Clear offscreen buffer
    ssd1306_DrawBitmap(0, 0, ShopWorldSprite, 128, 64, White);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, White);
    ssd1306_UpdateScreen();
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, KikiDownSprite, 9, 11, Black);

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = 20;  // Update every 20 ms

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            shopPlayerMovement();
            shopPlayerAction();
            shopDisplay();
            lastFrameTime = now;
        }
        HAL_Delay(1);  // Short delay to yield CPU time

        // Exit if player moves into the exit zone
        if (player.coordinates.y >= 62) {
            player.inWorld = CROP;
            break;
        }
    }
}
