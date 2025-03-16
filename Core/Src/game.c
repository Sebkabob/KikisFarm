#include "main.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "NimaLTD.I-CUBE-EE24_conf.h"
#include "ee24.h"
#include <math.h>
#include <stdio.h>


// Global brightness settings (adjust ranges as needed)
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 250

int brightness = 50;  // Default brightness value

int statbarShow = 0;
int batteryLife;
int leaveWorld = 0;
int refresh = 0;
int soundOn = 0;
int batteryPercent;
int batteryLow = 0;

int A_Button_Flag = 0;
int B_Button_Flag = 0;
int START_Button_Flag = 0;
int SELECT_Button_Flag = 0;
int UP_Button_Flag = 0;
int DOWN_Button_Flag = 0;
int LEFT_Button_Flag = 0;
int RIGHT_Button_Flag = 0;

int FrameRate = 20;	//lower is faster
int GrowSpeed = 1;

int worldBreak = 0;

int refreshBackground = 0;

Player player;

Game game;
/*                   ITEM         SELL  BUY    GROW XP   LV  TYPE       CROP SPRITE     ITEM ICON            TITLE            */
Item wheat       = { WHEAT,       5,    0,     3,   6,   0,  HCROP,     WheatSprite,    ItemIconWheat,       WheatTitle       };
Item corn        = { CORN,        7,    0,     8,   10,  0,  HCROP,     CornSprite,     ItemIconCorn,        CornTitle        };
Item potato      = { POTATO,      12,   0,     12,  28,  0,  HCROP,     PotatoSprite,   ItemIconPotato,      PotatoTitle      };
Item carrot      = { CARROT,      18,   0,     15,  50,  0,  HCROP,     CarrotSprite,   ItemIconCarrot,      CarrotTitle      };
Item pumpkin     = { PUMPKIN,     25,   0,     22,  70,  0,  HCROP,     PumpkinSprite,  ItemIconPumpkin,     PumpkinTitle     };
Item sugar       = { SUGAR,       45,   0,     25,  85,  0,  HCROP,     SugarSprite,    ItemIconSugar,       SugarTitle       };
Item mint        = { MINT,        35,   0,     15,  65,  0,  HCROP,     MintSprite,     ItemIconMint,        MintTitle        };
Item saffron     = { SAFFRON,     35,   0,     15,  65,  0,  HCROP,     SaffronSprite,  ItemIconSaffron,     SaffronTitle     };

Item wheatSeed   = { WHEATSEED,   5,    30,    0,   0,   1,  CROPSEED,  NULL,           WheatSeedSprite,     WheatSeedsTitle  };
Item cornSeed    = { CORNSEED,    20,   75,    0,   0,   3,  CROPSEED,  NULL,           CornSeedSprite,      CornSeedsTitle   };
Item potatoSeed  = { POTATOSEED,  50,   125,   0,   0,   6,  CROPSEED,  NULL,           PotatoSeedSprite,    PotatoSeedsTitle };
Item carrotSeed  = { CARROTSEED,  60,   210,   0,   0,   10, CROPSEED,  NULL,           CarrotSeedSprite,    CarrotSeedsTitle };
Item pumpkinSeed = { PUMPKINSEED, 80,   300,   0,   0,   12, CROPSEED,  NULL,           PumpkinSeedSprite,   PumpkinSeedsTitle};
Item mintSeed    = { MINTSEED,    90,   375,   0,   0,   14, CROPSEED,  NULL,           MintSeedSprite,      MintSeedsTitle   };
Item sugarSeed   = { SUGARSEED,   125,  450,   0,   0,   16, CROPSEED,  NULL,           SugarSeedSprite,     SugarSeedsTitle  };
Item saffronSeed = { SAFFRONSEED, 200,  600,   0,   0,   18, CROPSEED,  NULL,           SaffronSeedSprite,   SaffronSeedsTitle};

Item tillSoil    = { TILLSOIL,    0,    100,   0,   0,   1,  SERVICE,   NULL,           TillSprite,          TillMoreSoilTitle };

Item houseKey    = { HOUSEKEY,    9999, 90000, 0,   0,   20, ITEM,      NULL,           HouseKeySprite,      HouseKeyTitle    };

//Item coffee    = { COFFEE,      30,   100,  0,   0,   4,  CONSUMABLE,NULL,           NULL,                NULL };

uint32_t cropPlantTimes[10] = {0}; // Stores planting timestamps

CropTile cropTiles[10] = {
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, true  },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false }
};

// Define the shop inventory array as before.
Item shopItems[12];

void initShopItems(void) {
    shopItems[0]  = wheatSeed;
    shopItems[1]  = cornSeed;
    shopItems[2]  = potatoSeed;
    shopItems[3]  = carrotSeed;
    shopItems[4]  = pumpkinSeed;
    shopItems[5]  = mintSeed;
    shopItems[6]  = sugarSeed;
    shopItems[7]  = saffronSeed;
    shopItems[8]  = tillSoil;
    shopItems[9]  = houseKey;
    shopItems[10] = tillSoil;
    shopItems[11] = tillSoil;
    shopItems[12] = tillSoil;
}

Item* getItemPointerFromID(ItemType id) {
    switch (id) {
        case WHEAT:       return &wheat;
        case CORN:        return &corn;
        case POTATO:      return &potato;
        case CARROT:      return &carrot;
        case PUMPKIN:     return &pumpkin;
        case MINT:        return &mint;
        case SUGAR:       return &sugar;
        case SAFFRON:     return &saffron;
        case WHEATSEED:   return &wheatSeed;
        case CORNSEED:    return &cornSeed;
        case POTATOSEED:  return &potatoSeed;
        case CARROTSEED:  return &carrotSeed;
        case PUMPKINSEED: return &pumpkinSeed;
        case MINTSEED:    return &mintSeed;
        case SUGARSEED:   return &sugarSeed;
        case SAFFRONSEED: return &saffronSeed;
        case TILLSOIL:    return &tillSoil;
        case HOUSEKEY:    return &houseKey;
        default:          return NULL;
    }
}

Item getGrownCrop(ItemType seedId) {
    switch (seedId) {
        case WHEATSEED:   return wheat;
        case CORNSEED:    return corn;
        case POTATOSEED:  return potato;
        case CARROTSEED:  return carrot;
        case PUMPKINSEED: return pumpkin;
        case MINTSEED:    return mint;
        case SUGARSEED:   return sugar;
        case SAFFRONSEED: return saffron;
        default:
            // Return an "empty" crop when the seedId is invalid.
            // You might alternatively handle this as an error.
            {
                Item empty = {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
                return empty;
            }
    }
}


int getTillSoilCost() {
    int baseCost = 100;
    double multiplier = 2.0; // Increased from 1.85 to 2.0 for higher cost per additional spot.
    int cost = (int)(baseCost * pow(multiplier, player.soilSpots));
    return ((cost + 5) / 10) * 10;  // Round to the nearest 10.
}

void displayLevelUp() {
    int rectWidth = 80, rectHeight = 26;
    int rectX = (128 - rectWidth) / 2, rectY = (64 - rectHeight) / 2;
    ssd1306_FillRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, Black);
    ssd1306_DrawRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, White);

    const char *headerText = "Level Up!";
    int headerX = rectX + (rectWidth - strlen(headerText) * 7) / 2;
    int headerY = rectY + 3;
    ssd1306_SetCursor(headerX, headerY);
    // Casting headerText to (char*) to match the expected parameter type.
    ssd1306_WriteString((char*)headerText, Font_7x10, White);

    char leftStr[12], rightStr[12];
    // Use snprintf to avoid potential buffer overflow.
    snprintf(leftStr, sizeof(leftStr), "%d", player.level);
    snprintf(rightStr, sizeof(rightStr), "%d", player.level + 1);

    int totalWidth = strlen(leftStr) * 7 + 12 + strlen(rightStr) * 7 + 8;
    int startX = rectX + (rectWidth - totalWidth) / 2;
    int levelY = headerY + 12;
    ssd1306_SetCursor(startX, levelY);
    ssd1306_WriteString(leftStr, Font_7x10, White);
    ssd1306_DrawBitmap(startX + strlen(leftStr) * 7 + 4, levelY, Arrow, 12, 9, White);
    ssd1306_SetCursor(startX + strlen(leftStr) * 7 + 4 + 12 + 4, levelY);
    ssd1306_WriteString(rightStr, Font_7x10, White);
}

int gameLevelUp(void) {
    int baseXp = 150 + (70 * player.level) + (15 * player.level * player.level);
    double xpMultiplier = 1 + 0.35 * player.level;
    int xpNeededForNextLevel = baseXp * xpMultiplier;

    if (player.xp >= xpNeededForNextLevel) {
        displayLevelUp();
        player.level++;
        player.xp = 0;
        ssd1306_UpdateScreen();
        sound(levelUp);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1);
        while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    }
    return xpNeededForNextLevel;
}

void gameStartup(){
	pullEEPROM();
	//refreshInventory(player.inventory);
}

void initGame(){
    player.coordinates.x = 60;
    player.coordinates.y = 10;
    player.direction = DOWN;

    player.money = 9999999;
    player.inWorld = CROP;
    player.xp = 0;
    player.level = 20;
    player.soilSpots = 1;

    game.houseUnlocked = 0;
    game.cropHouseLights = 0;
    game.cropHouseIntro = 1;

    // Initialize player's inventory to empty.
    for (int i = 0; i < 9; i++) {
        player.inventory[i].item = NULL;
        player.inventory[i].quantity = 0;
    }
    // Give the player 1 wheat seed in slot 0.
    player.inventory[0].item = &wheatSeed;
    player.inventory[0].quantity = 1;

    // Initialize crop tiles.
    for (int i = 0; i < 10; i++) {
        cropTiles[i].crop.id = NONE;
        cropTiles[i].crop.sellValue = 0;
        cropTiles[i].crop.buyValue = 0;
        cropTiles[i].crop.growTime = 0;
        cropTiles[i].crop.xp = 0;
        cropTiles[i].crop.levelUnlock = 0;
        cropTiles[i].crop.subType = 0;
        cropTiles[i].crop.cropSprite = NULL;
        cropTiles[i].crop.itemSprite = NULL;
        cropTiles[i].grown = 0;
        cropTiles[i].isTilled = (i == 2) ? true : false;
    }
}

void updateButtonFlags(){
    // A button
    if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
    	A_Button_Flag = 1;
    }

    // B Button
    if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
    	B_Button_Flag = 1;
    }

    // START button
    if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
    	START_Button_Flag = 1;
    }

    // SELECT button
    if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
    	SELECT_Button_Flag = 1;
    }

    // UP button
    if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
    	UP_Button_Flag = 1;
    }

	// DOWN button
    if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
    	DOWN_Button_Flag = 1;
    }

    // LEFT button
    if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
    	LEFT_Button_Flag = 1;
    }

    // RIGHT button
    if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
    	RIGHT_Button_Flag = 1;
    }
}

void cropGrowth(){
    uint32_t currentTime = HAL_GetTick();
    // Loop through all 10 crop spots.
    for (int i = 0; i < 10; i++) {
        // Process only tilled spots with a planted crop that hasn't grown yet.
        if (cropTiles[i].isTilled && cropTiles[i].crop.id != NONE && cropTiles[i].grown == 0) {
            // The crop's growTime is in game ticks (seconds).
            // Compare elapsed time (in ms) with growTime (converted to ms).
            if (currentTime - cropPlantTimes[i] >= (cropTiles[i].crop.growTime * 1000) / GrowSpeed) {
            	refreshBackground = 1;
                cropTiles[i].grown = 1;
                // (Optional: update the sprite or trigger additional logic here.)
            }
        }
    }
}

void gameLogic(){
	gameLevelUp();
	cropGrowth();
	updateBatteryLife();
}

void playerDisplay(){
    // Choose the appropriate player sprite.
    const unsigned char *sprite;
    switch(player.direction) {
        case DOWN:  sprite = KikiDownSprite;  break;
        case UP:    sprite = KikiUpSprite;    break;
        case LEFT:  sprite = KikiLeftSprite;  break;
        case RIGHT: sprite = KikiRightSprite; break;
        default:    sprite = KikiDownSprite;  break;
    }

    // Draw the player sprite.
    ssd1306_FillRectangle(player.coordinates.x + 2, player.coordinates.y + 2,
    		              player.coordinates.x + 5, player.coordinates.y + 5, Black);

    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, sprite, 9, 11, White);


}

void playerErase(){
    // Choose the appropriate player sprite.
    const unsigned char *sprite;
    switch(player.direction) {
        case DOWN:  sprite = KikiDownSprite;  break;
        case UP:    sprite = KikiUpSprite;    break;
        case LEFT:  sprite = KikiLeftSprite;  break;
        case RIGHT: sprite = KikiRightSprite; break;
        default:    sprite = KikiDownSprite;  break;
    }

    // Draw the player sprite.
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, sprite, 9, 11, Black);
}


void textSpeaking(const char *text, int voice, int speed, int button) {
	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    int length = strlen(text);
    int maxLength = 60;
    if (length > maxLength) length = maxLength;

    int startY;

    if (length <= 20) {
        startY = 54;  // Only bottom line
    } else if (length <= 40) {
        startY = 44;  // Bottom two lines
    } else {
        startY = 34;  // Full textbox
    }

    ssd1306_FillRectangle(0, startY - 2, 128, 64, Black); // Clear textbox area
    ssd1306_DrawRectangle(0, startY - 2, 128 - 1, 64 - 1, White); // Draw textbox border
    ssd1306_UpdateScreen();

    int x = 4, y = startY; // Start position for text
    int charCount = 0;
    int skip = 0; // Flag for skipping animation

    ssd1306_SetCursor(x, y);

    for (int i = 0; i < length; i++) {
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && button) {
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            skip = 1;  // User pressed A, skip animation
            break;
        }

        if (text[i] != ' ') {
            buzzer(voice, 150 / speed);
        }

        ssd1306_WriteChar(text[i], Font_6x8, White);
        ssd1306_UpdateScreen();
        HAL_Delay(200 / speed);

        charCount++;

        if (charCount >= 20) {
            y += 10;
            x = 4;
            ssd1306_SetCursor(x, y);
            charCount = 0;
        }
    }

    if (skip) {
        // Instantly display full text
        ssd1306_FillRectangle(0, startY - 2, 128, 64, Black);
        ssd1306_DrawRectangle(0, startY - 2, 128 - 1, 64 - 1, White);

        x = 4;
        y = startY;
        charCount = 0;

        ssd1306_SetCursor(x, y);
        for (int i = 0; i < length; i++) {
            ssd1306_WriteChar(text[i], Font_6x8, White);
            charCount++;

            if (charCount >= 20) {
                y += 10;
                x = 4;
                ssd1306_SetCursor(x, y);
                charCount = 0;
            }
        }

        ssd1306_UpdateScreen();
    }

    while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1 && button);
}

void cutToDark(int speed){
    int mid = 32; // Screen midpoint (assuming 128x64 display)

    for (int i = 0; i < mid; i++) {
        ssd1306_Line(0, i, 127, i, Black); // Draw top to mid
        ssd1306_Line(0, 63 - i, 127, 63 - i, Black); // Draw bottom to mid
        ssd1306_UpdateScreen();
        HAL_Delay(speed);
    }
    HAL_Delay(speed * 10);
}

void displayStats(void) {
    char money[16];
    char level[16];

    // Calculate XP required for the next level
    int xpNeededForNextLevel = gameLevelUp();

    // Ensure we don’t divide by zero
    if (xpNeededForNextLevel == 0) xpNeededForNextLevel = 1;

    // Scale XP bar to a max width of 38 pixels (adjusted for the new bar size)
    int levelBar = (player.xp * 38) / xpNeededForNextLevel;
    if (levelBar > 38) levelBar = 38; // Clamp to max width

    // Draw the stats background in the lower 9 pixels (Y:55-64)
    ssd1306_FillRectangle(0, 55, 128, 64, White);

    // Convert player's money (in cents) to dollars and cents
    int dollars = player.money / 100;
    int cents = player.money % 100;
    // Format the money with leading zeros: e.g., "$08.50"
    sprintf(money, "$%05d.%02d", dollars, cents);
    ssd1306_SetCursor(73, 56);
    ssd1306_WriteString(money, Font_6x8, Black);

    // Prepare and draw level text
    sprintf(level, "Lv.%02d", player.level);
    ssd1306_SetCursor(1, 56);
    ssd1306_WriteString(level, Font_6x8, Black);

    // Draw XP bar background (empty bar)
    ssd1306_FillRectangle(32, 57, 70, 61, Black);

    // Fill XP progress in the bar (if progress exists)
    if (levelBar > 0) {
        ssd1306_FillRectangle(33, 58, 33 + levelBar, 60, White);
    }
}



int gameMenu(){
    int menuselect = 1;
    sound(menuOpen);

    // Clear only the menu area (y=15 to 48) so we can use the top area for battery info
    ssd1306_FillRectangle(0, 15, 127, 48, Black);
    ssd1306_Line(0, 15, 128, 15, White);
    ssd1306_Line(0, 48, 128, 48, White);

    // Draw menu items
    ssd1306_SetCursor(43, 18);
    ssd1306_WriteString("options", Font_6x8, White);
    ssd1306_SetCursor(37, 28);
    ssd1306_WriteString("main menu", Font_6x8, White);
    ssd1306_SetCursor(37, 38);
    ssd1306_WriteString("save game", Font_6x8, White);

    // Update battery life and display it in the top right corner (y ~2)
    int batteryVoltagePercentage = updateBatteryLife();
    char battStr[16];
    sprintf(battStr, "%d%%", batteryVoltagePercentage);
    ssd1306_SetCursor(8, 28);
    ssd1306_WriteString(battStr, Font_6x8, White);

    // Draw the initial selection rectangle around the "options" option
    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, White);
    ssd1306_UpdateScreen();
    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, Black);

    while(1){
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
            if (menuselect > 1) menuselect--;
            while(HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
            switch (menuselect){
                case 1:
                    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, Black);
                    break;
                case 2:
                    ssd1306_DrawRectangle(35, 27, 91, 36, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27, 91, 36, Black);
                    break;
                case 3:
                    ssd1306_DrawRectangle(35, 27+10, 91, 36+10, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27+10, 91, 36+10, Black);
                    break;
            }
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
            if (menuselect < 3) menuselect++;
            while(HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
            switch (menuselect){
                case 1:
                    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, Black);
                    break;
                case 2:
                    ssd1306_DrawRectangle(35, 27, 91, 36, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27, 91, 36, Black);
                    break;
                case 3:
                    ssd1306_DrawRectangle(35, 27+10, 91, 36+10, White);
                    ssd1306_UpdateScreen();
                    ssd1306_DrawRectangle(35, 27+10, 91, 36+10, Black);
                    break;
            }
        }

        if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
            sound(menuExit);
            while(HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
            break;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 1) {
            sound(menuSelect);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            gameOptions();
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 2) {
            sound(menuSelect);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            player.inWorld = TITLE;
            return 1;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 3) {
            sound(savingGame);
            ssd1306_FillRectangle(0, 16, 127, 47, Black);
            ssd1306_SetCursor(15, 27);
            ssd1306_WriteString("saving game...", Font_7x10, White);
            ssd1306_UpdateScreen();
            pushEEPROM();
            HAL_Delay(250);
            ssd1306_SetCursor(25, 27);
            ssd1306_FillRectangle(0, 16, 127, 47, Black);
            ssd1306_WriteString("game saved!", Font_7x10, White);
            ssd1306_UpdateScreen();
            sound(gameSaved);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1 && HAL_GPIO_ReadPin(GPIOB, B_Pin) == 1);
            HAL_Delay(100);
            return 0;
        }
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            sound(menuExit);
            while(HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }
        if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
            //something
        }
    }
    return 0;
}

void gameOptions(void) {
    int optionSelect = 1; // 1: sound, 2: brightness

    // Clear the options area and draw borders
    ssd1306_FillRectangle(0, 15, 127, 48, Black);
    ssd1306_Line(0, 15, 128, 15, White);
    ssd1306_Line(0, 48, 128, 48, White);

    ssd1306_UpdateScreen();

    ssd1306_SetCursor(43, 18);
    ssd1306_WriteString("options", Font_6x8, White);

    // Draw the two options
    ssd1306_SetCursor(35, 28);
    if (soundOn)
        ssd1306_WriteString("sound: on ", Font_6x8, White);
    else
        ssd1306_WriteString("sound: off", Font_6x8, White);
    ssd1306_SetCursor(35, 38);  // Moved one pixel to the right from x=34 to x=35
    ssd1306_WriteString("brightness", Font_6x8, White);

    // Draw selection rectangle (one pixel smaller on the y-axis)
    if (optionSelect == 1)
        ssd1306_DrawRectangle(32, 27, 97, 36, White);
    else if (optionSelect == 2)
        ssd1306_DrawRectangle(32, 37, 97, 46, White);
    ssd1306_UpdateScreen();

    while (1) {
        // UP button: move selection up if possible
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
            if (optionSelect > 1)
                optionSelect--;
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
            // Redraw options with new selection
            ssd1306_FillRectangle(0, 15, 127, 48, Black);
            ssd1306_Line(0, 15, 128, 15, White);
            ssd1306_Line(0, 48, 128, 48, White);
            ssd1306_SetCursor(43, 18);
            ssd1306_WriteString("options", Font_6x8, White);
            ssd1306_SetCursor(35, 28);
            if (soundOn)
                ssd1306_WriteString("sound: on ", Font_6x8, White);
            else
                ssd1306_WriteString("sound: off", Font_6x8, White);
            ssd1306_SetCursor(35, 38);
            ssd1306_WriteString("brightness", Font_6x8, White);
            if (optionSelect == 1)
                ssd1306_DrawRectangle(32, 27, 97, 36, White);
            else
                ssd1306_DrawRectangle(32, 37, 97, 46, White);
            ssd1306_UpdateScreen();
        }

        // DOWN button: move selection down if possible
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
            if (optionSelect < 2)
                optionSelect++;
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
            ssd1306_FillRectangle(0, 15, 127, 48, Black);
            ssd1306_Line(0, 15, 128, 15, White);
            ssd1306_Line(0, 48, 128, 48, White);
            ssd1306_SetCursor(43, 18);
            ssd1306_WriteString("options", Font_6x8, White);
            ssd1306_SetCursor(35, 28);
            if (soundOn)
                ssd1306_WriteString("sound: on ", Font_6x8, White);
            else
                ssd1306_WriteString("sound: off", Font_6x8, White);
            ssd1306_SetCursor(35, 38);
            ssd1306_WriteString("brightness", Font_6x8, White);
            if (optionSelect == 1)
                ssd1306_DrawRectangle(32, 27, 97, 36, White);
            else
                ssd1306_DrawRectangle(32, 37, 97, 46, White);
            ssd1306_UpdateScreen();
        }

        // A button: toggle sound if selected or increase brightness when brightness is selected
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            sound(menuSelect);
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            if (optionSelect == 1) {
                soundOn = !soundOn;
            } else if (optionSelect == 2) {
                brightness += 50;
                if (brightness > MAX_BRIGHTNESS)
                    brightness = 0;
                ssd1306_SetContrast(brightness);
            }
            // Redraw options after change
            ssd1306_FillRectangle(0, 15, 127, 48, Black);
            ssd1306_Line(0, 15, 128, 15, White);
            ssd1306_Line(0, 48, 128, 48, White);
            ssd1306_SetCursor(43, 18);
            ssd1306_WriteString("options", Font_6x8, White);
            ssd1306_SetCursor(35, 28);
            if (soundOn)
                ssd1306_WriteString("sound: on ", Font_6x8, White);
            else
                ssd1306_WriteString("sound: off", Font_6x8, White);
            ssd1306_SetCursor(35, 38);
            ssd1306_WriteString("brightness", Font_6x8, White);
            if (optionSelect == 1)
                ssd1306_DrawRectangle(32, 27, 97, 36, White);
            else
                ssd1306_DrawRectangle(32, 37, 97, 46, White);
            ssd1306_UpdateScreen();
        }

        // LEFT/RIGHT buttons adjust brightness if brightness option is selected.
        if (optionSelect == 2 && HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
            sound(menuSelect);
            if (brightness > MIN_BRIGHTNESS) {
                brightness -= 50;
                ssd1306_SetContrast(brightness);
            }
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (optionSelect == 2 && HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
            sound(menuSelect);
            if (brightness < MAX_BRIGHTNESS) {
                brightness += 50;
            }
            ssd1306_SetContrast(brightness);
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }

        // B button: exit the options submenu
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            sound(menuExit);
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }
    }

    // After exiting, redraw the main menu elements.
    ssd1306_FillRectangle(0, 15, 127, 48, Black);
    ssd1306_Line(0, 15, 128, 15, White);
    ssd1306_Line(0, 48, 128, 48, White);
    ssd1306_SetCursor(37, 38);
    ssd1306_WriteString("save game", Font_6x8, White);
    ssd1306_SetCursor(37, 28);
    ssd1306_WriteString("main menu", Font_6x8, White);
    ssd1306_SetCursor(43, 18);
    ssd1306_WriteString("options", Font_6x8, White);
    ssd1306_DrawRectangle(35, 17, 91, 26, White);
    ssd1306_UpdateScreen();
    ssd1306_DrawRectangle(35, 17, 91, 26, Black);
}

