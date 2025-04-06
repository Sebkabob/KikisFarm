#include "main.h"
#include "game.h"
#include "crop.h"
#include "sound.h"
#include "sprites.h"
#include "NimaLTD.I-CUBE-EE24_conf.h"
#include "ee24.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


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

int FrameRate = 22;	//lower is faster
int GrowSpeed = 1; //higher is faster

int worldBreak = 0;

int refreshBackground = 0;

Player player;

Pet cat;

Game game;

//------------------------------------------------------------------------------
// Item definitions for all items in the game.
//------------------------------------------------------------------------------
/*                     ITEM           SELL  BUY    GROW XP   LV   TYPE       CROP SPRITE     ITEM ICON            TITLE            */
Item wheat         = { WHEAT,         5,    0,     2,   5,   0,   HCROP,     WheatSprite,    ItemIconWheat,       WheatTitle       };
Item corn          = { CORN,          7,    0,     5,   10,  0,   HCROP,     CornSprite,     ItemIconCorn,        CornTitle        };
Item potato        = { POTATO,        12,   0,     8,   20,  0,   HCROP,     PotatoSprite,   ItemIconPotato,      PotatoTitle      };
Item carrot        = { CARROT,        15,   0,     12,  30,  0,   HCROP,     CarrotSprite,   ItemIconCarrot,      CarrotTitle      };
Item tomato        = { TOMATO,        18,   0,     16,  40,  0,   HCROP,     TomatoSprite,   ItemIconTomato,      TomatoTitle      };
Item pumpkin       = { PUMPKIN,       25,   0,     22,  55,  0,   HCROP,     PumpkinSprite,  ItemIconPumpkin,     PumpkinTitle     };
Item mint          = { MINT,          20,   0,     18,  50,  0,   HCROP,     MintSprite,     ItemIconMint,        MintTitle        };
Item sugar         = { SUGAR,         14,   0,     4,   22,  0,   HCROP,     SugarSprite,    ItemIconSugar,       SugarTitle       };
Item saffron       = { SAFFRON,       35,   0,     15,  55,  0,   HCROP,     SaffronSprite,  ItemIconSaffron,     SaffronTitle     };

Item wheatSeed     = { WHEATSEED,     50,   95,    0,   0,   1,   CROPSEED,  NULL,           WheatSeedSprite,     WheatSeedsTitle  };
Item cornSeed      = { CORNSEED,      100,  185,   0,   0,   2,   CROPSEED,  NULL,           CornSeedSprite,      CornSeedsTitle   };
Item potatoSeed    = { POTATOSEED,    150,  275,   0,   0,   4,   CROPSEED,  NULL,           PotatoSeedSprite,    PotatoSeedsTitle };
Item carrotSeed    = { CARROTSEED,    200,  380,   0,   0,   6,   CROPSEED,  NULL,           CarrotSeedSprite,    CarrotSeedsTitle };
Item tomatoSeed    = { TOMATOSEED,    225,  425,   0,   0,   9,   CROPSEED,  NULL,           TomatoSeedSprite,    TomatoSeedsTitle };
Item pumpkinSeed   = { PUMPKINSEED,   250,  475,   0,   0,   11,  CROPSEED,  NULL,           PumpkinSeedSprite,   PumpkinSeedsTitle};
Item mintSeed      = { MINTSEED,      275,  550,   0,   0,   13,  CROPSEED,  NULL,           MintSeedSprite,      MintSeedsTitle   };
Item sugarSeed     = { SUGARSEED,     350,  675,   0,   0,   15,  CROPSEED,  NULL,           SugarSeedSprite,     SugarSeedsTitle  };
Item saffronSeed   = { SAFFRONSEED,   500,  1000,  0,   0,   17,  CROPSEED,  NULL,           SaffronSeedSprite,   SaffronSeedsTitle};

Item tillSoil      = { TILLSOIL,      0,    100,   0,   0,   1,   SERVICE,   NULL,           TillSprite,          TillMoreSoilTitle };

Item houseKey      = { HOUSEKEY,      50000,90000, 0,   0,   20,  ITEM,      NULL,           HouseKeySprite,      HouseKeyTitle    };

/*                     ITEM           SELL  BUY    GROW XP  YIELD TYPE       CROP SPRITE     ITEM ICON            TITLE            */
Item apple         = { APPLE,         5,    0,     20,  200,  6,  HFRUIT,    AppleTreeSprite, AppleItemSprite,    AppleTitle       };
Item orange        = { ORANGE,        12,   0,     35,  400,  4,  HFRUIT,    OrangeTreeSprite,OrangeItemSprite,   OrangeTitle      };
Item banana        = { BANANA,        25,   0,     50,  600,  4,  HFRUIT,    BananaTreeSprite,BananaItemSprite,   BananaTitle      };
Item cherry        = { CHERRY,        20,   0,     80,  800,  17, HFRUIT,    CherryTreeSprite,CherryItemSprite,   CherryTitle      };
Item money         = { MONEY,         100,  0,     100, 400,  5,  HFRUIT,    MoneyTreeSprite, MoneyItemSprite,    MoneyTitle       };

Item appleSapling  = { APPLESAPLING,  15000,20000, 0,   0,   20,  SAPLING,   NULL,           AppleSaplingSprite,  AppleSaplingTitle  };
Item orangeSapling = { ORANGESAPLING, 30000,40000, 0,   0,   22,  SAPLING,   NULL,           OrangeSaplingSprite, OrangeSaplingTitle   };
Item bananaSapling = { BANANASAPLING, 40000,50000, 0,   0,   24,  SAPLING,   NULL,           BananaSaplingSprite, BananaSaplingTitle };
Item cherrySapling = { CHERRYSAPLING, 50000,60000, 0,   0,   26,  SAPLING,   NULL,           CherrySaplingSprite, CherrySaplingTitle };
Item moneySapling  = { MONEYSAPLING,  50000,99999, 0,   0,   28,  SAPLING,   NULL,           MoneySaplingSprite,  MoneySaplingTitle };

Item boat		   = { BOAT,  		  90000,600000,0,   0,   34,  ITEM,  	 NULL,           BoatItemSprite,  	 BoatTitle };


uint32_t cropPlantTimes[10] = {0}; // Stores planting time stamps
uint32_t treePlantTimes[6]  = {0}; // Stores planting time stamps

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

TreeTile treeTiles[6] = {
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false },
    { {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL}, 0, false }
};

// Define the shop inventory array as before.
Item shopItems[18];

//------------------------------------------------------------------------------
//Intilizes what items are in the shop and in what locations.
//------------------------------------------------------------------------------
void initShopItems(void) {
    shopItems[0]  = wheatSeed;
    shopItems[1]  = cornSeed;
    shopItems[2]  = potatoSeed;
    shopItems[3]  = carrotSeed;
    shopItems[4]  = tomatoSeed;
    shopItems[5]  = pumpkinSeed;
    shopItems[6]  = mintSeed;
    shopItems[7]  = sugarSeed;
    shopItems[8]  = saffronSeed;
    shopItems[9]  = tillSoil;
    shopItems[10] = houseKey;
    shopItems[11] = tillSoil;
    shopItems[12] = appleSapling;
    shopItems[13] = orangeSapling;
    shopItems[14] = bananaSapling;
    shopItems[15] = cherrySapling;
    shopItems[16] = moneySapling;
    shopItems[17] = boat;
}

//------------------------------------------------------------------------------
// Assigns pointers to item IDs.
//------------------------------------------------------------------------------
Item* getItemPointerFromID(ItemType id) {
    switch (id) {
        case WHEAT:         return &wheat;
        case CORN:          return &corn;
        case POTATO:        return &potato;
        case CARROT:        return &carrot;
        case TOMATO:        return &tomato;
        case PUMPKIN:       return &pumpkin;
        case MINT:          return &mint;
        case SUGAR:         return &sugar;
        case SAFFRON:       return &saffron;
        case WHEATSEED:     return &wheatSeed;
        case CORNSEED:      return &cornSeed;
        case POTATOSEED:    return &potatoSeed;
        case CARROTSEED:    return &carrotSeed;
        case TOMATOSEED:    return &tomatoSeed;
        case PUMPKINSEED:   return &pumpkinSeed;
        case MINTSEED:      return &mintSeed;
        case SUGARSEED:     return &sugarSeed;
        case SAFFRONSEED:   return &saffronSeed;
        case TILLSOIL:      return &tillSoil;
        case HOUSEKEY:      return &houseKey;
        case APPLE:         return &apple;
        case ORANGE:        return &orange;
        case BANANA:        return &banana;
        case CHERRY:        return &cherry;
        case MONEY:         return &money;
        case APPLESAPLING:  return &appleSapling;
        case ORANGESAPLING: return &orangeSapling;
        case BANANASAPLING: return &bananaSapling;
        case CHERRYSAPLING: return &cherrySapling;
        case MONEYSAPLING:  return &moneySapling;
        case BOAT:  		return &boat;

        default:            return NULL;
    }
}

//------------------------------------------------------------------------------
// Returns a non pointer for crops.
//------------------------------------------------------------------------------
Item getGrownCrop(ItemType seedId) {
    switch (seedId) {
        case WHEATSEED:   return wheat;
        case CORNSEED:    return corn;
        case POTATOSEED:  return potato;
        case CARROTSEED:  return carrot;
        case TOMATOSEED:  return tomato;
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

//------------------------------------------------------------------------------
// Returns a non pointer for saplings.
//------------------------------------------------------------------------------
Item getGrownSapling(ItemType saplingId) {
    switch (saplingId) {
        case APPLESAPLING:  return apple;
        case ORANGESAPLING: return orange;
        case BANANASAPLING: return banana;
        case CHERRYSAPLING: return cherry;
        case MONEYSAPLING:  return money;
        default:
            Item empty = {NONE, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL};
            return empty;
    }
}

//------------------------------------------------------------------------------
// Gets the cost of tilling soil for the shop.
//------------------------------------------------------------------------------
int getTillSoilCost() {
    // Use the power law equation for cost
    double cost = 100 * 2.3357 * pow(player.soilSpots, 2.4636);
    cost += 100;
    // Round the cost to the nearest multiple of 10
    return ((int)(cost + 5) / 10) * 10;
}

//------------------------------------------------------------------------------
// Displays when the player level ups and plays a confetti animation.
//------------------------------------------------------------------------------
typedef struct {
    int x;
    int y;
    int speed;
} Confetti;

void displayLevelUp() {
    // Level up box dimensions.
    int rectWidth = 80, rectHeight = 26;
    int rectX = (128 - rectWidth) / 2, rectY = (64 - rectHeight) / 2;

    // Prepare confetti particles.
    #define CONFETTI_COUNT 70
    Confetti confetti[CONFETTI_COUNT];
    for (int i = 0; i < CONFETTI_COUNT; i++) {
        confetti[i].x = rand() % 128;
        confetti[i].y = rand() % 64;  // Start anywhere on screen.
        confetti[i].speed = (rand() % 2) + 1;  // Speed: 1 or 2 pixels per frame.
    }

    // Animate confetti and level-up box until any button is pressed.
    while (1) {
        // Clear the screen.
        ssd1306_Fill(Black);

        // Update and draw each confetti piece.
        for (int i = 0; i < CONFETTI_COUNT; i++) {
            ssd1306_DrawPixel(confetti[i].x, confetti[i].y, White);
            confetti[i].y += confetti[i].speed;
            // Reset to top if the confetti falls off the bottom.
            if (confetti[i].y >= 64) {
                confetti[i].y = 0;
                confetti[i].x = rand() % 128;
            }
        }

        ORBuffer();

        // Draw the level-up box behind the text.
        ssd1306_FillRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, Black);
        ssd1306_DrawRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, White);

        // Draw the "Level Up!" header centered inside the box.
        const char *headerText = "Level Up!";
        int headerX = rectX + (rectWidth - strlen(headerText) * 7) / 2;
        int headerY = rectY + 3;
        ssd1306_SetCursor(headerX, headerY);
        ssd1306_WriteString((char*)headerText, Font_7x10, White);

        // Draw the level numbers and arrow.
        char leftStr[12], rightStr[12];
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

        // Update the display.
        ssd1306_UpdateScreen();
        HAL_Delay(50);

        // Check if any button is pressed.
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 ||
            HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 ) {
            // Wait until all buttons are released.
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 ||
                   HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 ) {
                HAL_Delay(10);
            }
            break;  // Exit the loop when any button is pressed.
        }
    }
}

//------------------------------------------------------------------------------
// Handles leveling up logic.
//------------------------------------------------------------------------------
int gameLevelUp(void) {
    int baseXp = 150 + (70 * player.level) + (15 * player.level * player.level);
    double xpMultiplier = 1 + 0.45 * player.level;
    int xpNeededForNextLevel = baseXp * xpMultiplier;

    if (player.xp >= xpNeededForNextLevel) {
        sound(levelUp);
        ssd1306_CopyBuffer();
        displayLevelUp();
        player.level++;
        player.xp = 0;
        ssd1306_UpdateScreen();
    }
    return xpNeededForNextLevel;
}

//------------------------------------------------------------------------------
// Handles what happens when the game starts up.
//------------------------------------------------------------------------------
void gameStartup(){
	pullEEPROM();
	resetPlayerHistory();
}

//------------------------------------------------------------------------------
// Dictates what the values are for the game when a new game begins.
//------------------------------------------------------------------------------
void initGame(){
    player.coordinates.x = 60;
    player.coordinates.y = 14;
    player.direction = DOWN;

    player.money = 999999;
    player.inWorld = CROP;
    player.xp = 0;
    player.level = 99;
    player.soilSpots = 16;

    game.houseUnlocked = 0;
    game.mileStone = CAT_MET - 1;
    game.cropHouseIntro = 1;

    game.day = 1;
    game.ticks = 0;

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

    for (int i = 0; i < 6; i++) {
        treeTiles[i].tree.id = NONE;
        treeTiles[i].tree.sellValue = 0;
        treeTiles[i].tree.buyValue = 0;
        treeTiles[i].tree.growTime = 0;
        treeTiles[i].tree.xp = 0;
        treeTiles[i].tree.levelUnlock = 0;
        treeTiles[i].tree.subType = 0;
        treeTiles[i].tree.cropSprite = NULL;
        treeTiles[i].tree.itemSprite = NULL;
        treeTiles[i].grown = 0;
        treeTiles[i].isTilled = false;
    }

    cat.coordinates.x = 55;
    cat.coordinates.y = 45;
    cat.direction = DOWN;
    cat.inWorld = ORCHARD;
    cat.love = 0;
    cat.sit = 0;
}

//------------------------------------------------------------------------------
// Updates button flags when a button is pressed.
//------------------------------------------------------------------------------
void updateButtonFlags(){
	// A button
	if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
	    A_Button_Flag = 1;
	} else {
	    A_Button_Flag = 0;
	}

	// B button
	if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
	    B_Button_Flag = 1;
	} else {
	    B_Button_Flag = 0;
	}

	// START button (assuming pressed state is logic 1)
	if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
	    START_Button_Flag = 1;
	} else {
	    START_Button_Flag = 0;
	}

	// SELECT button
	if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
	    SELECT_Button_Flag = 1;
	} else {
	    SELECT_Button_Flag = 0;
	}

	// UP button
	if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
	    UP_Button_Flag = 1;
	} else {
	    UP_Button_Flag = 0;
	}

	// DOWN button
	if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
	    DOWN_Button_Flag = 1;
	} else {
	    DOWN_Button_Flag = 0;
	}

	// LEFT button
	if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
	    LEFT_Button_Flag = 1;
	} else {
	    LEFT_Button_Flag = 0;
	}

	// RIGHT button
	if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
	    RIGHT_Button_Flag = 1;
	} else {
	    RIGHT_Button_Flag = 0;
	}

}

//------------------------------------------------------------------------------
// An animation that makes the screen go dark.
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// An animation that makes the screen go dark.
//------------------------------------------------------------------------------
void TransitionVortex(int speed) {
    int top = 0;
    int bottom = 63;  // 64-pixel tall display: 0..63
    int left = 0;
    int right = 127;  // 128-pixel wide display: 0..127
    while (top <= bottom && left <= right) {
        ssd1306_Line(left, top, right, top, Black);
        ssd1306_Line(left, bottom, right, bottom, Black);
        ssd1306_Line(left, top, left, bottom, Black);
        ssd1306_Line(right, top, right, bottom, Black);
        ssd1306_UpdateScreen();
        HAL_Delay(speed);
        top++; bottom--; left++; right--;
    }
    HAL_Delay(speed * 10);
}

//------------------------------------------------------------------------------
// Handles crop growing logic.
//------------------------------------------------------------------------------
void cropGrowth(){
    uint32_t currentTime = HAL_GetTick();
    for (int i = 0; i < 10; i++) {
        if (cropTiles[i].isTilled && cropTiles[i].crop.id != NONE && cropTiles[i].grown == 0) {
            if (currentTime - cropPlantTimes[i] >= (cropTiles[i].crop.growTime * 1000) / GrowSpeed) {
            	refreshBackground = 1;
                cropTiles[i].grown = 1;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Handles tree growing logic.
//------------------------------------------------------------------------------
void treeGrowth(void) {
    uint32_t currentTime = HAL_GetTick();
    for (int i = 0; i < 6; i++) {
        if (treeTiles[i].isTilled && treeTiles[i].tree.id != NONE) {
            if (treeTiles[i].grown == 0) {
                if (currentTime - treePlantTimes[i] >= 1440000) { // 24 minutes to grow to sapling
                    treeTiles[i].grown = 1;
                    treePlantTimes[i] = currentTime;  // Reset timer for next stage
                }
            } else if (treeTiles[i].grown == 1) {
                if (currentTime - treePlantTimes[i] >= 720000) { // 12 minutes to grow to tree
                    treeTiles[i].grown = 2;
                    treePlantTimes[i] = currentTime;
                }
            } else if (treeTiles[i].grown == 2) {
                if (currentTime - treePlantTimes[i] >= (treeTiles[i].tree.growTime * 1000) / GrowSpeed) {
                    treeTiles[i].grown = 3;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Handles the time keeping logic for the game.
//------------------------------------------------------------------------------
void gameTime() {
    static uint32_t lastTick = 0;
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastTick >= 1000) {
        game.ticks++;
        lastTick = currentTime;

        if (game.ticks >= 1440) { // 24 minutes in a day
            game.day++;
            game.ticks = 0;
        }
    }
}

//------------------------------------------------------------------------------
// Displays the cat Love bar.
//------------------------------------------------------------------------------
void displayCatStats(void) {
	ssd1306_FillRectangle(0, 55, 128, 64, White);
    ssd1306_SetCursor(15, 56);
    ssd1306_WriteString("Love", Font_6x8, Black);
    ssd1306_FillRectangle(43, 57, 113, 61, Black);
    int levelBar = (cat.love-CAT_LOVE_THRESHOLD) / 60;
    if (levelBar >= 68)
    	ssd1306_FillRectangle(44, 58, 112, 60, White);
    else if (levelBar <= 0){
    	ssd1306_FillRectangle(44, 58, 112, 60, Black);
    }
    else if (levelBar != 0)
    	ssd1306_FillRectangle(44, 58, 44 + levelBar, 60, White);
}

//------------------------------------------------------------------------------
// Handles feeding the pet when you are close to it.
//------------------------------------------------------------------------------
void petFeed() {
	if (game.mileStone < CAT_MET){
		textSpeaking("Dang, this cat looks super hungry       ", 110, 7, 1);
		textSpeaking("Looks like nobody   fed him for awhile", 110, 7, 1);
		textSpeaking("Maybe I should feed him something...", 110, 7, 1);
		textSpeaking("(press A to feed the cat)", 110, 7, 1);
		textSpeaking("(hold B to make him sit)", 110, 7, 1);
		game.mileStone = CAT_MET;
	}
    // Check if the player is near the cat.
    if (abs(player.coordinates.x - cat.coordinates.x) <= 10 &&
        abs(player.coordinates.y - cat.coordinates.y) <= 10 && player.inWorld == cat.inWorld) {
    	if (cat.sit == 1 && cat.love > CAT_LOVE_THRESHOLD){
    		resetPlayerHistory();
    		sound(catStand);
    		cat.sit = 0;
    	}
        showInventory(3);
    }
}

//------------------------------------------------------------------------------
// Handles how the pets love decreases with time when not fed.
//------------------------------------------------------------------------------
void petLove() {
    static uint32_t lastTime = 0;
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastTime >= 1000 && cat.love > 2 && cat.sit == 0) {
        cat.love -= 2;
        lastTime = currentTime;
    }
    if (cat.love < 2){
    	cat.sit = 1;
    }
}

//------------------------------------------------------------------------------
// Handles if the pet is told to sit.
//------------------------------------------------------------------------------
void petSit(void) {
    if (abs(player.coordinates.x - cat.coordinates.x) <= 10 &&
        abs(player.coordinates.y - cat.coordinates.y) <= 10) {;
    	if (cat.sit == 1 && cat.love > CAT_LOVE_THRESHOLD && player.inWorld == cat.inWorld) {
    		resetPlayerHistory();
        	sound(catStand);
    		cat.sit = 0;
    	} else if (cat.sit == 0) {
    		sound(catSit);
    		cat.sit = 1;
    		cat.direction = DOWN;
    	}
    }
}


//------------------------------------------------------------------------------
// Calls different game logic that happens constantly.
//------------------------------------------------------------------------------
void gameLogic(){
	gameTime();
	gameLevelUp();
	cropGrowth();
	treeGrowth();
	petLove();
}

//------------------------------------------------------------------------------
// Displays the map and allows for interaction with the map.
//------------------------------------------------------------------------------
void theMap(){
	sound(mapOpen);
	World worldToGoTo;
	int worldSelect = 0;

	  switch (player.inWorld) {
	  case CROP:
		  worldSelect = 2;
		  break;
	  case SHOP:
		  worldSelect = 1;
		  break;
	  case CROPHOUSE:
		  worldSelect = 2;
		  break;
	  case BASEMENT:
		  worldSelect = 2;
		  break;
	  case HOUSEROOM:
		  worldSelect = 2;
		  break;
	  case TITLE:
		  worldSelect = 2;
		  break;
	  case ORCHARD:
		  worldSelect = 3;
		  break;
	  case FISHING:
		  worldSelect = 4;
		  break;
        }
	while(1){
	    // A button
	    if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
	    	leaveWorld = 1;
            player.coordinates.x = 60;
            player.coordinates.y = 36;
            if (game.mileStone < BOAT_ACQUIRED && worldToGoTo == FISHING){
            	textSpeaking("hah! nice try...    get that boat first", 100, 8, 1);
            	break;
            }
	    	player.inWorld = worldToGoTo;
	    	TransitionVortex(10);
	    	HAL_Delay(50);
	    	break;
	    }

	    // B Button
	    if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
	    	break;
	    }

	    // START button
	    if (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
	    	while(HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
	    	break;
	    }

	    // SELECT button
	    if (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 1);
	    	break;
	    }

	    // UP button
	    if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
	    	sound(menuNav);
	    	if (worldSelect > 1)
	    		worldSelect--;
	    }

		// DOWN button
	    if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
	    	sound(menuNav);
	    	if (worldSelect < 3)
	    		worldSelect++;
	    }

	    // LEFT button
	    if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
	    	sound(menuNav);
	    	if (worldSelect > 1)
	    		worldSelect--;
	    }

	    // RIGHT button
	    if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
	    	while(HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
	    	sound(menuNav);
	    	if (worldSelect < 3)
	    		worldSelect++;
	    }
	    ssd1306_Fill(Black);
	    ssd1306_DrawBitmap(0, 0, TheMap, 128, 64, White);
		switch (worldSelect) {
		case 1:
			ssd1306_DrawBitmap(128-62, 0, ShopsTitle, 62, 27, White);
			ssd1306_DrawBitmap(30, 6, KikiBackErase, 9, 11, Black);
			ssd1306_DrawBitmap(30, 6, KikiDownSprite, 9, 11, White);
			worldToGoTo = SHOP;
			break;
		case 2:
			ssd1306_DrawBitmap(128-62, 0, CropFarmTitle, 62, 27, White);
			ssd1306_DrawBitmap(35, 38, KikiBackErase, 9, 11, Black);
			ssd1306_DrawBitmap(35, 38, KikiDownSprite, 9, 11, White);
			worldToGoTo = CROP;
			break;
		case 3:
			ssd1306_DrawBitmap(128-62, 0, OrchardTitle, 62, 27, White);
			ssd1306_DrawBitmap(75, 44, KikiBackErase, 9, 11, Black);
			ssd1306_DrawBitmap(75, 44, KikiDownSprite, 9, 11, White);
			worldToGoTo = ORCHARD;
			break;
		case 4:
			ssd1306_DrawBitmap(128-62, 0, FishingTitle, 62, 27, White);
			ssd1306_DrawBitmap(118, 47, KikiBackErase, 9, 11, Black);
			ssd1306_DrawBitmap(118, 47, KikiDownSprite, 9, 11, White);
			worldToGoTo = FISHING;
			break;
	        }


	    ssd1306_UpdateScreen();
	}
}

//------------------------------------------------------------------------------
// Shows text with the whole screen.
//------------------------------------------------------------------------------
void textSpeakingFullScreen(const char *text, int voice, int speed, int button) {
    while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    int length = strlen(text);
    int maxLength = 120;
    if (length > maxLength) length = maxLength;
    ssd1306_FillRectangle(0, 0, 128, 64, Black);
    ssd1306_DrawRectangle(0, 0, 127, 63, White);
    ssd1306_UpdateScreen();
    int x = 4, y = 4;
    int max_line_chars = 20;
    int current_line_chars = 0;
    int skip = 0;
    ssd1306_SetCursor(x, y);
    int i = 0;
    while (i < length) {
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && button) {
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            skip = 1;
            break;
        }
        if (text[i] == ' ') {
            if (current_line_chars + 1 > max_line_chars) {
                y += 10;
                x = 4;
                ssd1306_SetCursor(x, y);
                current_line_chars = 0;
            }
            ssd1306_WriteChar(' ', Font_6x8, White);
            ssd1306_UpdateScreen();
            HAL_Delay(200 / speed);
            current_line_chars++;
            i++;
            continue;
        }
        int wordStart = i;
        int wordLength = 0;
        while ((i + wordLength) < length && text[i + wordLength] != ' ') {
            wordLength++;
        }
        if (current_line_chars > 0 && (current_line_chars + 1 + wordLength) > max_line_chars) {
            y += 10;
            x = 4;
            ssd1306_SetCursor(x, y);
            current_line_chars = 0;
        }
        for (int j = 0; j < wordLength; j++) {
            if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && button) {
                while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
                skip = 1;
                break;
            }
            if (text[wordStart + j] != ' ') {
                buzzer(voice, 150 / speed);
            }
            ssd1306_WriteChar(text[wordStart + j], Font_6x8, White);
            ssd1306_UpdateScreen();
            HAL_Delay(200 / speed);
            current_line_chars++;
        }
        i = wordStart + wordLength;
        if (skip) break;
        if (i < length && text[i] == ' ') {
            if (current_line_chars + 1 > max_line_chars) {
                y += 10;
                x = 4;
                ssd1306_SetCursor(x, y);
                current_line_chars = 0;
            } else {
                ssd1306_WriteChar(' ', Font_6x8, White);
                ssd1306_UpdateScreen();
                HAL_Delay(200 / speed);
                current_line_chars++;
            }
            while (i < length && text[i] == ' ') {
                i++;
            }
        }
    }
    if (skip) {
        ssd1306_FillRectangle(0, 0, 128, 64, Black);
        ssd1306_DrawRectangle(0, 0, 127, 63, White);
        ssd1306_UpdateScreen();
        x = 4; y = 4; current_line_chars = 0;
        ssd1306_SetCursor(x, y);
        i = 0;
        while (i < length) {
            if (text[i] == ' ') {
                if (current_line_chars + 1 > max_line_chars) {
                    y += 10;
                    x = 4;
                    ssd1306_SetCursor(x, y);
                    current_line_chars = 0;
                }
                ssd1306_WriteChar(' ', Font_6x8, White);
                current_line_chars++;
                i++;
                continue;
            }
            int wordStart = i;
            int wordLength = 0;
            while ((i + wordLength) < length && text[i + wordLength] != ' ') {
                wordLength++;
            }
            if (current_line_chars > 0 && (current_line_chars + 1 + wordLength) > max_line_chars) {
                y += 10;
                x = 4;
                ssd1306_SetCursor(x, y);
                current_line_chars = 0;
            }
            for (int j = 0; j < wordLength; j++) {
                ssd1306_WriteChar(text[wordStart + j], Font_6x8, White);
                current_line_chars++;
            }
            i = wordStart + wordLength;
            if (i < length && text[i] == ' ') {
                if (current_line_chars + 1 > max_line_chars) {
                    y += 10;
                    x = 4;
                    ssd1306_SetCursor(x, y);
                    current_line_chars = 0;
                } else {
                    ssd1306_WriteChar(' ', Font_6x8, White);
                    current_line_chars++;
                }
                while (i < length && text[i] == ' ') {
                    i++;
                }
            }
        }
        ssd1306_UpdateScreen();
    }
    while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1 && button);
}

//------------------------------------------------------------------------------
// Shows a question and prompts the user for a yes or no, returns the answer.
//------------------------------------------------------------------------------
int textPrompt(const char *headerText) {
	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    ssd1306_Fill(Black);
    int maxCharsPerLine = 18;
    int textLength = strlen(headerText);
    int numLines = (textLength + maxCharsPerLine - 1) / maxCharsPerLine;
    if (numLines > 4) numLines = 4;
    int headerAreaHeight = 42;
    int startY = (headerAreaHeight - (numLines * 10)) / 2;
    for (int i = 0; i < numLines; i++) {
        int startIndex = i * maxCharsPerLine;
        int remaining = textLength - startIndex;
        int lineLength = (remaining < maxCharsPerLine) ? remaining : maxCharsPerLine;
        char lineBuffer[19];
        strncpy(lineBuffer, headerText + startIndex, lineLength);
        lineBuffer[lineLength] = '\0';
        int lineWidth = lineLength * 7;
        int startX = (128 - lineWidth) / 2;
        ssd1306_SetCursor(startX, startY + i * 10);
        ssd1306_WriteString(lineBuffer, Font_7x10, White);
    }
    ssd1306_SetCursor(20, 50);
    ssd1306_WriteString("yes", Font_7x10, White);
    ssd1306_SetCursor(94, 50);
    ssd1306_WriteString("no", Font_7x10, White);
    int selection = 0;
    ssd1306_DrawRectangle(92, 48, 109, 62, White);
    ssd1306_UpdateScreen();
    while (1) {
        HAL_Delay(10);
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
                HAL_Delay(10);
            }
            if (selection != 0) {
                ssd1306_DrawRectangle(18, 48, 42, 62, Black);
                ssd1306_DrawRectangle(92, 48, 109, 62, White);
                selection = 0;
                ssd1306_UpdateScreen();
            }
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
                HAL_Delay(10);
            }
            if (selection != 1) {
                ssd1306_DrawRectangle(92, 48, 109, 62, Black);
                ssd1306_DrawRectangle(18, 48, 42, 62, White);
                selection = 1;
                ssd1306_UpdateScreen();
            }
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
                HAL_Delay(10);
            }
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return (selection == 1) ? 1 : 0;
        }
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
                HAL_Delay(10);
            }
            ssd1306_Fill(Black);
            ssd1306_UpdateScreen();
            return 0;
        }
    }
}

//------------------------------------------------------------------------------
// A dynamic text box that shows text with a box varying in size depending on length.
//------------------------------------------------------------------------------
void textSpeaking(const char *text, int voice, int speed, int button) {
	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    int length = strlen(text);
    int maxLength = 60;
    if (length > maxLength) length = maxLength;
    int startY;
    if (length <= 20) {
        startY = 54;
    } else if (length <= 40) {
        startY = 44;
    } else {
        startY = 34;
    }
    ssd1306_FillRectangle(0, startY - 2, 128, 64, Black);
    ssd1306_DrawRectangle(0, startY - 2, 128 - 1, 64 - 1, White);
    ssd1306_UpdateScreen();
    int x = 4, y = startY;
    int charCount = 0;
    int skip = 0;
    ssd1306_SetCursor(x, y);
    for (int i = 0; i < length; i++) {
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && button) {
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
            skip = 1;
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

//------------------------------------------------------------------------------
// Displays the players stats.
//------------------------------------------------------------------------------
void displayStats(void) {
    char money[16];
    char level[16];
    int xpNeededForNextLevel = gameLevelUp();
    if (xpNeededForNextLevel == 0) xpNeededForNextLevel = 1;
    int levelBar = (player.xp * 37) / xpNeededForNextLevel;
    if (levelBar > 37) levelBar = 37;
    ssd1306_FillRectangle(0, 55, 128, 64, White);
    int dollars = player.money / 100;
    int cents = player.money % 100;
    sprintf(money, "$%05d.%02d", dollars, cents);
    ssd1306_SetCursor(73, 56);
    ssd1306_WriteString(money, Font_6x8, Black);
    sprintf(level, "Lv.%02d", player.level);
    ssd1306_SetCursor(1, 56);
    ssd1306_WriteString(level, Font_6x8, Black);
    ssd1306_FillRectangle(32, 57, 70, 61, Black);
    if (levelBar > 0) {
        ssd1306_FillRectangle(33, 58, 33 + levelBar, 60, White);
    }
}

//------------------------------------------------------------------------------
// Helper function that shows the battery and current game day.
//------------------------------------------------------------------------------
void drawMenuSideFeatures(){
    char daysLabel[] = "Day";
    int daysLabelWidth = strlen(daysLabel) * 6;
    int daysLabelX = 128 - daysLabelWidth - 8;
    ssd1306_SetCursor(daysLabelX, 22);
    ssd1306_WriteString(daysLabel, Font_6x8, White);
    char daysStr[16];
    sprintf(daysStr, "%d", game.day);
    int daysStrWidth = strlen(daysStr) * 6;
    int daysStrX = daysLabelX + (daysLabelWidth - daysStrWidth) / 2;
    ssd1306_SetCursor(daysStrX, 32);
    ssd1306_WriteString(daysStr, Font_6x8, White);
    ssd1306_DrawRectangle(7-1, 27-1, 27-1, 37-1, White);
    ssd1306_DrawRectangle(27-1, 30-1, 29-1, 34-1, White);
    int batteryVoltagePercentage = updateBatteryLife();
    char battStr[16];
    sprintf(battStr, "%02d%%", batteryVoltagePercentage);
    ssd1306_SetCursor(8, 28);
    ssd1306_WriteString(battStr, Font_6x8, White);
}

//------------------------------------------------------------------------------
// Draws the options in the menu.
//------------------------------------------------------------------------------
void menuItemsDraw(){
    // Clear only the menu area (y=15 to 48) so we can use the top area for battery info
    ssd1306_FillRectangle(0, 15, 127, 48, Black);
    ssd1306_Line(0, 15, 128, 15, White);
    ssd1306_Line(0, 48, 128, 48, White);

    drawMenuSideFeatures();

    // Draw menu items
    ssd1306_SetCursor(43, 18);
    ssd1306_WriteString("options", Font_6x8, White);
    ssd1306_SetCursor(37, 28);
    ssd1306_WriteString("main menu", Font_6x8, White);
    ssd1306_SetCursor(37, 38);
    ssd1306_WriteString("save game", Font_6x8, White);

    // Draw the initial selection rectangle around the "options" option
    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, White);
    ssd1306_UpdateScreen();
    ssd1306_DrawRectangle(35, 27-10, 91, 36-10, Black);
}

//------------------------------------------------------------------------------
// Displays the interactive game menu.
//------------------------------------------------------------------------------
int gameMenu(){
    sound(menuOpen);
    int menuselect = 1;

    menuItemsDraw();

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

    menuItemsDraw();
}

