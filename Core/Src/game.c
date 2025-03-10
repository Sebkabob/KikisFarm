#include "main.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "NimaLTD.I-CUBE-EE24_conf.h"
#include "ee24.h"
#include <math.h>

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

int refreshBackground;

Player player = { .inWorld = TITLE, .money = 12, .xp = 0, .level = 1, .soilSpots = 1};

Game game;

// Crops
/*               CROP     SELL BUY GROW XP  LV  TYPE   CROP SPRITE          ITEM ICON        */
Item wheat   = { WHEAT,   2,   0,  2,   2,  1,  HCROP, WheatSprite,   NULL, ItemIconWheat };
Item corn    = { CORN,    12,  0,  10,  9,  2,  HCROP, CornSprite,    NULL, ItemIconCorn };
Item potato  = { POTATO,  16,  0,  15,  20, 4,  HCROP, PotatoSprite,  NULL, ItemIconPotato };
Item carrot  = { CARROT,  24,  0,  20,  25, 7,  HCROP, CarrotSprite,  NULL, ItemIconCarrot };
Item pumpkin = { PUMPKIN, 40,  0,  28,  36, 11, HCROP, PumpkinSprite, NULL, ItemIconPumpkin };
Item sugar   = { SUGAR,   60,  0,  35,  50, 17, HCROP, SugarSprite,   NULL, ItemIconSugar };


// Seeds (linked to grown crops)
Item wheatSeed   = { WHEATSEED,   3,   12,   5,  4,  1,  SEED, NULL, WheatSeedSprite,   NULL };
Item cornSeed    = { CORNSEED,    4,   30,   10, 9,  3,  SEED, NULL, CornSeedSprite,    NULL };
Item potatoSeed  = { POTATOSEED,  8,   42,   15, 14, 5,  SEED, NULL, PotatoSeedSprite,  NULL };
Item carrotSeed  = { CARROTSEED,  12,  80,   20, 24, 9,  SEED, NULL, CarrotSeedSprite,  NULL };
Item pumpkinSeed = { PUMPKINSEED, 20,  150,  28, 36, 13, SEED, NULL, PumpkinSeedSprite, NULL };
Item sugarSeed   = { SUGARSEED,   30,  250,  35, 48, 19, SEED, NULL, SugarSeedSprite,   NULL };

// Other items
Item tillSoil = {TILLSOIL, 0, 100, 0, 100, 1, 0, TillSprite, TillSprite};
Item houseKey = {HOUSEKEY, 1, 15000, 0, 8000, 18, 0, HouseKeySprite, HouseKeySprite};

uint32_t cropPlantTimes[10] = {0}; // Define the array to store planting timestamps

CropTile cropTiles[10] = {
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, true },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false }
};

// Define the shop inventory
Item shopItems[8];

// Soil Spot Cost Function (Unchanged)
int getTillSoilCost() {
    int baseCost = 100;
    double multiplier = 1.45; // % increase per spot owned
    int cost = (int)(baseCost * pow(multiplier, player.soilSpots));
    return ((cost + 5) / 10) * 10;  // Round to the nearest 10
}

void displayLevelUp() {
    // Define the rectangle dimensions
    int rectWidth = 80;
    int rectHeight = 26;
    int rectX = (128 - rectWidth) / 2;  // Centers horizontally (128 is the screen width)
    int rectY = (64 - rectHeight) / 2;  // Centers vertically (64 is the screen height)

    // Clear the rectangle area by filling it with black and draw its border in white
    ssd1306_FillRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, Black);
    ssd1306_DrawRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, White);

    // Write the header "Level Up!" centered at the top of the rectangle
    const char *headerText = "Level Up!";
    int headerWidth = strlen(headerText) * 7;  // approximate width with Font_7x10
    int headerX = rectX + (rectWidth - headerWidth) / 2;
    int headerY = rectY + 3;  // Slight offset from the top
    ssd1306_SetCursor(headerX, headerY);
    ssd1306_WriteString(headerText, Font_7x10, White);

    // Prepare the strings for the old level and new level
    char leftStr[10], rightStr[10];
    sprintf(leftStr, "%d", player.level - 1);
    sprintf(rightStr, "%d", player.level);

    // Calculate widths of the text and arrow
    int leftWidth = strlen(leftStr) * 7;
    int rightWidth = strlen(rightStr) * 7;
    int arrowWidth = 12;  // given arrow width
    int gap = 4;  // gap between text and arrow

    // Total width of the three elements (old level, arrow, new level) plus gaps
    int totalWidth = leftWidth + gap + arrowWidth + gap + rightWidth;

    // Calculate the starting X coordinate to center the group within the rectangle
    int startX = rectX + (rectWidth - totalWidth) / 2;
    int levelY = headerY + 12;  // Placed below the header

    // Draw the old level string
    ssd1306_SetCursor(startX, levelY);
    ssd1306_WriteString(leftStr, Font_7x10, White);

    // Draw the arrow bitmap between the two numbers, adjusting Y to vertically center it (arrow height 9 vs. text height ~10)
    int arrowX = startX + leftWidth + gap;
    int arrowY = levelY;  // adjust as needed (e.g., levelY + 1) if you want a slight vertical offset
    ssd1306_DrawBitmap(arrowX, arrowY, Arrow, arrowWidth, 9, White);

    // Draw the new level string
    int rightX = arrowX + arrowWidth + gap;
    ssd1306_SetCursor(rightX, levelY);
    ssd1306_WriteString(rightStr, Font_7x10, White);
}



// Revised Level-Up Function
void gameLevelUp(){
    int baseXp = 100 + (50 * player.level) + (10 * player.level * player.level);
    double multiplier = 1 + 0.15 * player.level;
    int xpNeededForNextLevel = baseXp * multiplier;
    if (player.xp > xpNeededForNextLevel){
        player.level++;
        player.xp = 0;
        displayLevelUp();
    	ssd1306_UpdateScreen();
    	buzzer(700,80);
    	buzzer(800,80);
    	buzzer(900,80);
    	buzzer(1000,80);
    	while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1);
    	while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    }
}

void initGame(){
	player.money = 20;
	player.inWorld = CROP;
	player.xp = 0;
	player.level = 1;
	player.soilSpots = 1;
    for (int i = 0; i < 9; i++) {
        player.inventory[i].item = NULL;
        player.inventory[i].quantity = 0;
    }
    for (int i = 0; i < 10; i++) {
        cropTiles[i].crop.id = NONE;
        cropTiles[i].crop.sellValue = 0;
        cropTiles[i].crop.buyValue = 0;
        cropTiles[i].crop.growTime = 0;
        cropTiles[i].crop.xp = 0;
        cropTiles[i].crop.levelUnlock = 0;
        cropTiles[i].crop.subType = 0;
        cropTiles[i].crop.sprite = NULL;
        cropTiles[i].crop.seedSprite = NULL;
        cropTiles[i].grown = 0;
        // Set tile 3 (index 2) to be tilled, others not:
        cropTiles[i].isTilled = (i == 2) ? true : false;
    }
}

void initShopItems(void) {
    shopItems[0] = wheatSeed;
    shopItems[1] = cornSeed;
    shopItems[2] = potatoSeed;
    shopItems[3] = carrotSeed;
    shopItems[4] = pumpkinSeed;
    shopItems[5] = sugarSeed;
    shopItems[6] = tillSoil;
    shopItems[7] = houseKey;
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
            if (currentTime - cropPlantTimes[i] >= cropTiles[i].crop.growTime * 1000) {
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
    int xpNeededForNextLevel = 100 + (50 * player.level) + (10 * player.level * player.level);

    // Ensure we don’t divide by zero
    if (xpNeededForNextLevel == 0) xpNeededForNextLevel = 1;

    // Scale XP bar to a max width of 40 pixels
    int levelBar = (player.xp * 40) / xpNeededForNextLevel;
    if (levelBar > 40) levelBar = 40; // Clamp to max width

    // Draw the stats background in the lower 9 pixels (Y:55-64)
    ssd1306_FillRectangle(0, 55, 128, 64, White);

    // Prepare and draw money text
    sprintf(money, "$%07d", player.money);
    ssd1306_SetCursor(79, 56);
    ssd1306_WriteString(money, Font_6x8, Black);

    // Prepare and draw level text
    sprintf(level, "Lv.%02d", player.level);
    ssd1306_SetCursor(1, 56);
    ssd1306_WriteString(level, Font_6x8, Black);

    // Draw XP bar background (empty bar)
    ssd1306_FillRectangle(32, 57, 74, 61, Black);

    // Fill XP progress in the bar (if progress exists)
    if (levelBar > 0) {
        ssd1306_FillRectangle(33, 58, 33 + levelBar, 60, White);
    }
}

int gameMenu(){
    int menuselect = 1;
    buzzer(800,25);

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

//    // Update battery life and display it in the top right corner (y ~2)
//    int batteryVoltageHundred = updateBatteryLife();
//    int wholePart = batteryVoltageHundred / 100;      // Integer part (e.g., 3)
//    int fractionalPart = batteryVoltageHundred % 100;   // Fractional part (e.g., 45)
//    char battStr[16];
//    sprintf(battStr, "%d.%02dV", wholePart, fractionalPart);
//    ssd1306_SetCursor(2, 28);
//    ssd1306_WriteString(battStr, Font_6x8, White);

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
            buzzer(800,25);
            while(HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
            break;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 1) {
            buzzer(800,25);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            gameOptions();
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 2) {
            buzzer(800,25);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            player.inWorld = TITLE;
            return 1;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuselect == 3) {
            buzzer(800,25);
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
        	buzzer(300, 75);
        	buzzer(400, 75);
        	buzzer(500, 75);
        	buzzer(600, 75);
            while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1 && HAL_GPIO_ReadPin(GPIOB, B_Pin) == 1);
            buzzer(800,25);
            HAL_Delay(100);
            return 0;
        }
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            buzzer(800,25);
            while(HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            break;
        }
        if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
            //something
        }
    }
    ssd1306_Fill(Black);
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
            buzzer(800, 25);
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
            if (brightness > MIN_BRIGHTNESS) {
                brightness -= 50;
                ssd1306_SetContrast(brightness);
            }
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (optionSelect == 2 && HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
            if (brightness < MAX_BRIGHTNESS) {
                brightness += 50;
            }
            ssd1306_SetContrast(brightness);
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }

        // B button: exit the options submenu
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            buzzer(800, 25);
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

