#include "main.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "NimaLTD.I-CUBE-EE24_conf.h"
#include "ee24.h"

int statbarShow = 0;
int batteryLife;
int leaveWorld = 0;
int refresh = 0;
int soundOn = 0;

extern EE24_HandleTypeDef hee24;

Player player = { .inWorld = TITLE, .money = 5, .xp = 0, .level = 1, .soilSpots = 1};

Game game;

// Crops
Item wheat   = { WHEAT,   10, 20, 5, 6, 1, 0, WheatSprite,  NULL };
Item corn    = { CORN,    15, 30, 8, 12, 2, 0, CornSprite,   NULL };
Item potato  = { POTATO,  20, 40, 12, 18, 4, 0, PotatoSprite, NULL };
Item carrot  = { CARROT,  30, 60, 16, 30, 7, 0, CarrotSprite, NULL };
Item pumpkin = { PUMPKIN, 50, 100,20, 45, 11, 0, PumpkinSprite,NULL };
Item sugar   = { SUGAR,   75, 150,25, 60, 17, 0, SugarSprite,  NULL };

// Seeds (linked to grown crops)
Item wheatSeed   = { WHEATSEED,   3,  5, 5, 6, 1, 0, WheatSprite,  WheatSeedSprite };
Item cornSeed    = { CORNSEED,    5, 12, 8, 12, 2, 0, CornSprite,   CornSeedSprite };
Item potatoSeed  = { POTATOSEED, 10, 18,12, 18, 4, 0, PotatoSprite, PotatoSeedSprite };
Item carrotSeed  = { CARROTSEED, 20, 40,16, 30, 7, 0, CarrotSprite, CarrotSeedSprite };
Item pumpkinSeed = { PUMPKINSEED,30, 60,20, 45, 11, 0, PumpkinSprite,PumpkinSeedSprite };
Item sugarSeed   = { SUGARSEED,   50,100,25, 60, 17, 0, SugarSprite,  SugarSeedSprite };

// Other items
Item tillSoil = {TILLSOIL, 0, 100, 0, 100, 1, 0, TillSprite, TillSprite};
Item houseKey = {HOUSEKEY, 0, 9999, 0, 5000, 18, 0, HouseKeySprite, HouseKeySprite};

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

void initGame(){
	player.money = 5;
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
        cropTiles[i].crop.quantity = 0;
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

const unsigned char* getItemTitle(ItemType itemType) {
    switch (itemType) {
        case WHEATSEED: return WheatSeedsTitle;
        case CORNSEED: return CornSeedsTitle;
        case POTATOSEED: return PotatoSeedsTitle;
        case CARROTSEED: return CarrotSeedsTitle;
        case PUMPKINSEED: return PumpkinSeedsTitle;
        case SUGARSEED: return SugarSeedsTitle;
        case TILLSOIL: return TillMoreSoilTitle;
        case HOUSEKEY: return HouseKeyTitle;
        default: return NULL;
    }
}


//CropTile cropSpot1 = { .crop = NONE, .grown = 0};

void pushEEPROM(void) {
    uint32_t addr = 0;  // Starting address

    // Write soundOn to EEPROM
    if (!EE24_Write(&hee24, addr, (uint8_t *)&soundOn, sizeof(soundOn), 1000)) {
        // Debug: indicate failure (e.g., via UART, LED, or buzzer pattern)
        buzzer(100, 300);  // For instance, a low-pitched buzzer sound
    } else {

    }
    HAL_Delay(10);
    addr += sizeof(soundOn);

    // Write player info to EEPROM
    if (!EE24_Write(&hee24, addr, (uint8_t *)&player, sizeof(player), 1000)) {
        buzzer(100, 300);
    }

    HAL_Delay(10);
    addr += sizeof(cropTiles);

    // Write crop info to EEPROM
    if (!EE24_Write(&hee24, addr, (uint8_t *)&cropTiles, sizeof(cropTiles), 1000)) {
        buzzer(100, 300);
    }
    HAL_Delay(10);
}

void pullEEPROM(void) {
    uint32_t addr = 0;  // Starting address

    // Read soundOn from EEPROM
    if (!EE24_Read(&hee24, addr, (uint8_t *)&soundOn, sizeof(soundOn), 1000)) {
        buzzer(100, 300);
    }

    HAL_Delay(10);
    addr += sizeof(soundOn);

    // Read player's money from EEPROM
    if (!EE24_Read(&hee24, addr, (uint8_t *)&player, sizeof(player), 1000)) {
        buzzer(100, 300);
    }

    HAL_Delay(10);
    addr += sizeof(cropTiles);

    if (!EE24_Read(&hee24, addr, (uint8_t *)&cropTiles, sizeof(cropTiles), 1000)) {
        buzzer(100, 300);
    }
    HAL_Delay(10);
}


void gameLevelUp(){
	int xpNeededForNextLevel = 100 + (50 * player.level) + (10 * player.level * player.level);
	if (player.xp > xpNeededForNextLevel){
		player.level++;
		player.xp = 0;
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

// Add an item to the inventory
int addItemToInventory(InventorySlot inventory[], Item *item, int quantity) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item == NULL || inventory[i].item->id == NONE) {
            inventory[i].item = item;
            inventory[i].quantity = quantity;
            return 1; // Success
        }
        if (inventory[i].item->id == item->id) {
            inventory[i].quantity += quantity;
            return 1; // Success
        }
    }
    return 0; // Inventory full
}

// Remove an item from the inventory
int removeItemFromInventory(InventorySlot inventory[], ItemType itemType, int quantity) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == itemType) {
            if (inventory[i].quantity > quantity) {
                inventory[i].quantity -= quantity;
            } else {
                inventory[i].item = NULL;
                inventory[i].quantity = 0;
            }
            return 1; // Success
        }
    }
    return 0; // Item not found
}

// Check if an item exists in the inventory
int hasItemInInventory(InventorySlot inventory[], ItemType itemType) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == itemType) {
            return inventory[i].quantity;
        }
    }
    return 0; // Item not found
}

int showInventory(int plantSeed) {

    int itemSelect = 1; // Start with the first item selected
    int moved = 1;      // Indicates that the selection box needs to be redrawn
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0; // Track previous selection box position

    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

    // Draw the items in the inventory
    for (int i = 0; i < 9; i++) {
        if (player.inventory[i].item != NULL && player.inventory[i].item->id != NONE) {
            xMov = ((i % 3) * 17);
            yMov = ((i / 3) * 17);
            const unsigned char *spriteToDraw;
            if (player.inventory[i].item->seedSprite != NULL) {
                spriteToDraw = player.inventory[i].item->seedSprite;
            } else {
                spriteToDraw = player.inventory[i].item->sprite;
            }
            ssd1306_DrawBitmap(8 + xMov, 4 + yMov, spriteToDraw, 14, 14, White);
        }
    }

    displayStats();

    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;

    // Draw the initial selection box
    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);

    // Draw the title and quantity of the selected item
    if (player.inventory[itemSelect - 1].item != NULL && player.inventory[itemSelect - 1].item->id != NONE) {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
        const unsigned char *itemTitle = getItemTitle(player.inventory[itemSelect - 1].item->id);
        if (itemTitle != NULL) {
            ssd1306_DrawBitmap(57, 2, itemTitle, 65, 24, White);
        }
        char quantityText[10];
        snprintf(quantityText, sizeof(quantityText), "x%d", player.inventory[itemSelect - 1].quantity);
        ssd1306_SetCursor(90, 32);
        ssd1306_WriteString(quantityText, Font_6x8, White);
    } else {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
    }

    ssd1306_UpdateScreen();

    while (1) {
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0 && itemSelect > 3) {
            moved = 1;
            buzzer(540, 10);
            itemSelect -= 3;
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0 && itemSelect < 7) {
            moved = 1;
            buzzer(540, 10);
            itemSelect += 3;
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0 && (itemSelect % 3) != 1) {
            moved = 1;
            buzzer(540, 10);
            itemSelect -= 1;
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0 && (itemSelect % 3) != 0) {
            moved = 1;
            buzzer(540, 10);
            itemSelect += 1;
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }

        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 || HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 || HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
            break;
        }

        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

            if (plantSeed == 1) {
                // Check if the selected item is a valid seed
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem != NULL &&
                    (selectedItem->id == WHEATSEED ||
                     selectedItem->id == CORNSEED ||
                     selectedItem->id == POTATOSEED ||
                     selectedItem->id == CARROTSEED ||
                     selectedItem->id == PUMPKINSEED ||
                     selectedItem->id == SUGARSEED)) {

                    // Remove one unit of the seed from the inventory.
                	return selectedItem->id;
                } else {
                    // Optionally, you could provide feedback if the selected item is not a valid seed.
                }
            } else {
                // Handle normal inventory selection if needed.
                moved = 1; // Ensure redraw after selection.
            }
            moved = 1; // Ensure redraw after selection.
        }

        if (moved) {
            moved = 0;
            // Erase the previous selection box.
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);

            // Update selection box position.
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;

            displayStats();

            // Draw the new selection box.
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);

            // Clear previous text area.
            ssd1306_FillRectangle(57, 2, 121, 53, Black);

            // Draw title and quantity for the selected item.
            if (player.inventory[itemSelect - 1].item != NULL && player.inventory[itemSelect - 1].item->id != NONE) {
                const unsigned char *itemTitle = getItemTitle(player.inventory[itemSelect - 1].item->id);
                if (itemTitle != NULL) {
                    ssd1306_DrawBitmap(57, 2, itemTitle, 65, 24, White);
                }
                char quantityText[10];
                snprintf(quantityText, sizeof(quantityText), "x%d", player.inventory[itemSelect - 1].quantity);
                ssd1306_SetCursor(90, 32);
                ssd1306_WriteString(quantityText, Font_6x8, White);
            }

            ssd1306_UpdateScreen();
        }
    }
    // If no valid seed is selected, return NONE.
    return NONE;
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




void gameIntro(){
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("By 2097, Earth's farms were fully ruled by AI.", 100, 5, 1);
	textSpeaking("No longer did humans toil in the fields.", 100, 5, 1);
	textSpeaking("Food was plentiful. Life was easy.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("One day, a deadly blight spread across the land.", 100, 5, 1);
	textSpeaking("Crops withered. Harvests failed.", 100, 5, 1);
	textSpeaking("AI was helpless. It had no solution.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("Earth had lost all its farmers.", 100, 5, 1);
	textSpeaking("There was no one left to fix this.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("But a dying breed still remained...", 100, 5, 1);
	textSpeaking("Farming engineers rose from the ashes.", 100, 5, 1);
	textSpeaking("They built new robots unlike any before.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("These machines could till, plant, and harvest.", 100, 5, 1);
	textSpeaking("But AI could no longer be trusted.", 100, 5, 1);
	textSpeaking("Blight-resistant crops needed real oversight.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("Only humans could analyze the soil.", 100, 5, 1);
	textSpeaking("Only humans could adapt in real time.", 100, 5, 1);
	textSpeaking("The robots became tools, not rulers.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("You are one of the last farmers.", 100, 5, 1);
	textSpeaking("The survival of humanity is in your hands.", 100, 5, 1);
	textSpeaking("Start farming. You have no choice.", 100, 5, 1);

	ssd1306_Fill(Black);
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

    // Update battery life and display it in the top right corner (y ~2)
    int batteryVoltageHundred = updateBatteryLife();
    int wholePart = batteryVoltageHundred / 100;      // Integer part (e.g., 3)
    int fractionalPart = batteryVoltageHundred % 100;   // Fractional part (e.g., 45)

    char battStr[16];
    sprintf(battStr, "%d.%02dV", wholePart, fractionalPart);
    ssd1306_SetCursor(2, 28);
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


// Global brightness settings (adjust ranges as needed)
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 250
int brightness = 50;  // Default brightness value

void gameOptions(void) {
    int optionSelect = 1; // 1: sound, 2: brightness

    // Clear the options area and draw borders
    ssd1306_FillRectangle(0, 15, 127, 48, Black);
    ssd1306_Line(0, 15, 128, 15, White);
    ssd1306_Line(0, 48, 128, 48, White);
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
    ssd1306_Line(37, 41, 85, 41, White); // TEMP line
    ssd1306_SetCursor(37, 28);
    ssd1306_WriteString("main menu", Font_6x8, White);
    ssd1306_SetCursor(43, 18);
    ssd1306_WriteString("options", Font_6x8, White);
    ssd1306_DrawRectangle(35, 17, 91, 26, White);
    ssd1306_UpdateScreen();
    ssd1306_DrawRectangle(35, 17, 91, 26, Black);
}

