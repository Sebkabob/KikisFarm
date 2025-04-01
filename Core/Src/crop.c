#include "game.h"
#include "crop.h"
#include "inventory.h"
#include "sprites.h"
#include <stdbool.h>
#include <stdio.h>    // For sprintf
#include <string.h>   // For memset if needed

// Forward declarations (prototypes)
void drawSoil(void);
void drawCrops(void);
void cropDisplay(void);
void cropPlayerMovement(void);
void cropPlayerAction(void);
void handleCrop();

// Global variables
int showCoordinates = 0;

//------------------------------------------------------------------------------
// Returns true if the (x,y) point collides with any defined obstacle.
//------------------------------------------------------------------------------
bool cropObstacle(int x, int y) {
    return (
        (x >= OB1_X && x < OB1_X + OB1_W && y >= OB1_Y && y < OB1_Y + OB1_H) ||
        (x >= OB2_X && x < OB2_X + OB2_W && y >= OB2_Y && y < OB2_Y + OB2_H) ||
        (x >= OB3_X && x < OB3_X + OB3_W && y >= OB3_Y && y < OB3_Y + OB3_H) ||
        (x >= OB4_X && x < OB4_X + OB4_W && y >= OB4_Y && y < OB4_Y + OB4_H)
    );
}

//------------------------------------------------------------------------------
// Checks if the player is on one of the crop spots.
// Returns a number 1-10 if on a spot, 0 otherwise.
//------------------------------------------------------------------------------
int checkIfOnCrop(void) {
    // Check if the player is on a crop spot and if the spot is unlocked
    if (player.coordinates.x >= cropSpotXc1 - 3 && player.coordinates.x < cropSpotXc1 + 13 &&
        player.coordinates.y >= cropSpotYr1 - 3 && player.coordinates.y <= cropSpotYr1 + 4) {
        if (player.soilSpots > 7) // Spot 1 is unlocked if soilSpots > 7
            return 1;
    }
    if (player.coordinates.x >= cropSpotXc2 - 3 && player.coordinates.x < cropSpotXc2 + 13 &&
        player.coordinates.y >= cropSpotYr1 - 3 && player.coordinates.y <= cropSpotYr1 + 4) {
        if (player.soilSpots > 1) // Spot 2 is unlocked if soilSpots > 1
            return 2;
    }
    if (player.coordinates.x >= cropSpotXc3 - 3 && player.coordinates.x < cropSpotXc3 + 13 &&
        player.coordinates.y >= cropSpotYr1 - 3 && player.coordinates.y <= cropSpotYr1 + 4) {
        if (player.soilSpots > 0) // Spot 3 is unlocked if soilSpots > 0
            return 3;
    }
    if (player.coordinates.x >= cropSpotXc4 - 3 && player.coordinates.x < cropSpotXc4 + 13 &&
        player.coordinates.y >= cropSpotYr1 - 3 && player.coordinates.y <= cropSpotYr1 + 4) {
        if (player.soilSpots > 2) // Spot 4 is unlocked if soilSpots > 2
            return 4;
    }
    if (player.coordinates.x >= cropSpotXc5 - 3 && player.coordinates.x < cropSpotXc5 + 13 &&
        player.coordinates.y >= cropSpotYr1 - 3 && player.coordinates.y <= cropSpotYr1 + 4) {
        if (player.soilSpots > 6) // Spot 5 is unlocked if soilSpots > 6
            return 5;
    }
    if (player.coordinates.x >= cropSpotXc1 - 3 && player.coordinates.x < cropSpotXc1 + 13 &&
        player.coordinates.y >= cropSpotYr2 - 3 && player.coordinates.y <= cropSpotYr2 + 4) {
        if (player.soilSpots > 9) // Spot 6 is unlocked if soilSpots > 9
            return 6;
    }
    if (player.coordinates.x >= cropSpotXc2 - 3 && player.coordinates.x < cropSpotXc2 + 13 &&
        player.coordinates.y >= cropSpotYr2 - 3 && player.coordinates.y <= cropSpotYr2 + 4) {
        if (player.soilSpots > 5) // Spot 7 is unlocked if soilSpots > 5
            return 7;
    }
    if (player.coordinates.x >= cropSpotXc3 - 3 && player.coordinates.x < cropSpotXc3 + 13 &&
        player.coordinates.y >= cropSpotYr2 - 3 && player.coordinates.y <= cropSpotYr2 + 4) {
        if (player.soilSpots > 3) // Spot 8 is unlocked if soilSpots > 3
            return 8;
    }
    if (player.coordinates.x >= cropSpotXc4 - 3 && player.coordinates.x < cropSpotXc4 + 13 &&
        player.coordinates.y >= cropSpotYr2 - 3 && player.coordinates.y <= cropSpotYr2 + 4) {
        if (player.soilSpots > 4) // Spot 9 is unlocked if soilSpots > 4
            return 9;
    }
    if (player.coordinates.x >= cropSpotXc5 - 3 && player.coordinates.x < cropSpotXc5 + 13 &&
        player.coordinates.y >= cropSpotYr2 - 3 && player.coordinates.y <= cropSpotYr2 + 4) {
        if (player.soilSpots > 8) // Spot 10 is unlocked if soilSpots > 8
            return 10;
    }
    return 0; // Not on a crop spot or the spot is not unlocked
}

int checkIfNearHouse(){
    if (player.coordinates.x >= OB1_X && player.coordinates.x < OB1_X + OB1_W &&
        player.coordinates.y >= OB1_Y && player.coordinates.y <= OB1_Y + OB1_H + 2
		&& hasItemInInventory(player.inventory, HOUSEKEY) > 0) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------
// Draws soil based on the number of tilled spots.
//------------------------------------------------------------------------------
void drawSoil(void) {
    // Spot 3 -> corresponds to cropTiles[2]
    if (player.soilSpots > 0) {
        uint16_t color = (cropTiles[2].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc3, cropSpotYr1, TilledSoilSprite, 18, 15, color); // Spot 3
    }
    // Spot 2 -> corresponds to cropTiles[1]
    if (player.soilSpots > 1) {
        uint16_t color = (cropTiles[1].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc2, cropSpotYr1, TilledSoilSprite, 18, 15, color); // Spot 2
    }
    // Spot 4 -> corresponds to cropTiles[3]
    if (player.soilSpots > 2) {
        uint16_t color = (cropTiles[3].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc4, cropSpotYr1, TilledSoilSprite, 18, 15, color); // Spot 4
    }
    // Spot 8 -> corresponds to cropTiles[7]
    if (player.soilSpots > 3) {
        uint16_t color = (cropTiles[7].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc3, cropSpotYr2, TilledSoilSprite, 18, 15, color); // Spot 8
    }
    // Spot 9 -> corresponds to cropTiles[8]
    if (player.soilSpots > 4) {
        uint16_t color = (cropTiles[8].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc4, cropSpotYr2, TilledSoilSprite, 18, 15, color); // Spot 9
    }
    // Spot 7 -> corresponds to cropTiles[6]
    if (player.soilSpots > 5) {
        uint16_t color = (cropTiles[6].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc2, cropSpotYr2, TilledSoilSprite, 18, 15, color); // Spot 7
    }
    // Spot 5 -> corresponds to cropTiles[4]
    if (player.soilSpots > 6) {
        uint16_t color = (cropTiles[4].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc5, cropSpotYr1, TilledSoilSprite, 18, 15, color); // Spot 5
    }
    // Spot 1 -> corresponds to cropTiles[0]
    if (player.soilSpots > 7) {
        uint16_t color = (cropTiles[0].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc1, cropSpotYr1, TilledSoilSprite, 18, 15, color); // Spot 1
    }
    // Spot 10 -> corresponds to cropTiles[9]
    if (player.soilSpots > 8) {
        uint16_t color = (cropTiles[9].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc5, cropSpotYr2, TilledSoilSprite, 18, 15, color); // Spot 10
    }
    // Spot 6 -> corresponds to cropTiles[5]
    if (player.soilSpots > 9) {
        uint16_t color = (cropTiles[5].crop.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(cropSpotXc1, cropSpotYr2, TilledSoilSprite, 18, 15, color); // Spot 6
    }
}


//------------------------------------------------------------------------------
// Draws crops (currently only sugar).
//------------------------------------------------------------------------------
void drawCrops(void) {
    // Loop through all 10 crop spots
    for (int i = 0; i < 10; i++) {
        // Only draw if the tile is tilled and a crop has been planted
        if (cropTiles[i].isTilled && cropTiles[i].crop.id != NONE) {
            int x = 0, y = 0;
            // Determine drawing coordinates based on the crop spot number (i+1)
            switch (i + 1) {
                case 1:  x = cropSpotXc1; y = cropSpotYr1; break;
                case 2:  x = cropSpotXc2; y = cropSpotYr1; break;
                case 3:  x = cropSpotXc3; y = cropSpotYr1; break;
                case 4:  x = cropSpotXc4; y = cropSpotYr1; break;
                case 5:  x = cropSpotXc5; y = cropSpotYr1; break;
                case 6:  x = cropSpotXc1; y = cropSpotYr2; break;
                case 7:  x = cropSpotXc2; y = cropSpotYr2; break;
                case 8:  x = cropSpotXc3; y = cropSpotYr2; break;
                case 9:  x = cropSpotXc4; y = cropSpotYr2; break;
                case 10: x = cropSpotXc5; y = cropSpotYr2; break;
                default: break;
            }

            const unsigned char *spriteToDraw = NULL;
            if (cropTiles[i].grown == 1) {
                // Use getItemPointerFromID() to get the item pointer and then its sprite.
                Item *itemPtr = getItemPointerFromID(cropTiles[i].crop.id);
                if (itemPtr != NULL) {
                    spriteToDraw = itemPtr->cropSprite;
                }
            } else {
                // Crop is not fully grown: use the sprout sprite.
                spriteToDraw = SproutSprite;
            }

            ssd1306_DrawBitmap(x, y, spriteToDraw, 18, 15, White);
        }
    }
}


//------------------------------------------------------------------------------
// Full redraw of the scene: draws background, UI, and player sprite offscreen,
//------------------------------------------------------------------------------
void cropDisplay(void) {
    // Draw static background elements.
    if (!hasItemInInventory(player.inventory, HOUSEKEY))
    	ssd1306_DrawBitmap(0, 0, LockedHouseSprite, 37, 31, White);
    ssd1306_DrawBitmap(0, 0, CropWorldSprite, 128, 64, White);

    drawSoil();
    drawCrops();
}

//------------------------------------------------------------------------------
// Handles player movement. Reads button inputs and updates the player's
// position and direction. A short delay is used for debouncing.
//------------------------------------------------------------------------------
void cropPlayerMovement(void) {
    uint8_t step = 1;  // Fixed movement step

    // UP button
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE - 12 && !cropObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // DOWN button
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE && !cropObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // LEFT button
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE && !cropObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }

    // RIGHT button
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE && !cropObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}


void cropPlant(){
    refreshBackground = 1;
    int spot = checkIfOnCrop();  // Returns a number 1–10 if on a valid crop spot.
    // If no grown crop, allow planting if the spot is empty
    if (spot != 0 && cropTiles[spot - 1].crop.id == NONE) {
        sound(inventoryOpen);

        // Let the player select a seed from the inventory
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
        }
        ItemType seedId = showInventory(1);
        // If a valid seed id was returned, find the corresponding inventory slot
        if (seedId != NONE) {
            int slot = -1;
            for (int i = 0; i < 9; i++) {
                if (player.inventory[i].item != NULL &&
                    player.inventory[i].item->id == seedId) {
                    slot = i;
                    break;
                }
            }
            if (slot != -1) {
                // Use the helper function to convert the seed to its grown crop.
                Item crop = getGrownCrop(seedId);
                if (crop.id == NONE) {
                    // Handle error if the conversion failed.
                    return;
                }

                // Plant the corresponding crop on the crop tile
                cropTiles[spot - 1].crop = crop;
                cropTiles[spot - 1].grown = 0;
                cropTiles[spot - 1].isTilled = true;

                // Record the time when the crop was planted
                cropPlantTimes[spot - 1] = HAL_GetTick();

                // Remove one unit of the seed from the player's inventory
                removeItemFromInventory(player.inventory,
                                        player.inventory[slot].item->id,
                                        1);
            }
        }
    }
}



void cropHarvest(){
    refreshBackground = 1;
    int spot = checkIfOnCrop();  // Returns a number 1–10 if on a valid crop spot.

    if (spot != 0 && cropTiles[spot - 1].grown == 1) {
        // Play harvesting tones

        Item *harvestedCrop = NULL;
        switch (cropTiles[spot - 1].crop.id) {
            case WHEAT:       harvestedCrop = &wheat;   break;
            case CORN:        harvestedCrop = &corn;    break;
            case POTATO:      harvestedCrop = &potato;  break;
            case CARROT:      harvestedCrop = &carrot;  break;
            case TOMATO:      harvestedCrop = &tomato;  break;
            case PUMPKIN:     harvestedCrop = &pumpkin; break;
            case MINT:        harvestedCrop = &mint;    break;
            case SUGAR:       harvestedCrop = &sugar;   break;
            case SAFFRON:     harvestedCrop = &saffron; break;
            default:
                // If no matching crop, handle the error.
                return;
        }

        // If the inventory is full, play an error sound and exit.
        if (!addItemToInventory(player.inventory, harvestedCrop, 1)) {
        	sound(inventoryFull);
            return;
        }

    	sound(harvest);

        // Increase player's XP and reset crop state.
        player.xp += cropTiles[spot - 1].crop.xp;
        cropTiles[spot - 1].grown = 0;
        cropPlantTimes[spot - 1] = HAL_GetTick();

        ssd1306_UpdateScreen();
    }
}



void cropDestroy(){
	refreshBackground = 1;
    uint32_t startTime = HAL_GetTick();
    while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
        HAL_Delay(10);
        if (HAL_GetTick() - startTime >= 1500) {
            // Held for 1.5 seconds: destroy the crop.
            int spot = checkIfOnCrop();
            if (spot != 0 && cropTiles[spot - 1].crop.id != NONE) {
                cropTiles[spot - 1].crop.id = NONE;
                cropTiles[spot - 1].grown = 0;
                sound(destroy);
            }
            // Wait until button is released.
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            return;
        }
    }
}

//------------------------------------------------------------------------------
// Processes player actions (interacting with crops, toggling UI, menu).
// Basic debouncing is used by waiting for button release.
//------------------------------------------------------------------------------
void cropPlayerAction(void) {
    // Button A: Harvest a grown crop or plant a seed if the spot is empty.
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

        if (checkIfNearHouse()){
        	player.inWorld = CROPHOUSE;
            player.coordinates.x = 60;
            player.coordinates.y = 40;
            player.direction = UP;
            leaveWorld = 1;
        }

        int spot = checkIfOnCrop();  // Returns a number 1–10 if on a valid crop spot.
        if (spot != 0) {
            // If a grown crop exists, harvest it.
            if (cropTiles[spot - 1].grown == 1) {
                cropHarvest();
            }
            // Otherwise, if the spot is empty, plant a seed.
            else if (cropTiles[spot - 1].crop.id == NONE) {
                cropPlant();
            }
        }
    }

    // Button B: Either toggle the status bar (short press) or, if held for 2 seconds, destroy the crop.
    if (B_Button_Flag) {
    	B_Button_Flag = 0;
    	refreshBackground = 1;
        uint32_t startTime = HAL_GetTick();
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
            if (HAL_GetTick() - startTime >= 1500) {
                // Held for 1.5 seconds: destroy the crop.
                int spot = checkIfOnCrop();
                if (spot != 0 && cropTiles[spot - 1].crop.id != NONE) {
                    cropTiles[spot - 1].crop.id = NONE;
                    cropTiles[spot - 1].grown = 0;
                    sound(destroy);
                }
                // Wait until button is released.
                while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
                return;
            }
        }
        sound(inventoryOpen);
        showInventory(0);
    }

    // SELECT button: Enter menu mode.
    if (SELECT_Button_Flag) {
    	SELECT_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
        statbarShow = 0;
        refreshBackground = 1;
        sound(menuOpen);
        int p = gameMenu();  // Assume gameMenu() returns a flag.
        if (p)
            leaveWorld = 1;
    }

    // START button:
    if (START_Button_Flag) {
    	START_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
    }
}

//------------------------------------------------------------------------------
// Main loop for the crop world: sets initial position and then repeatedly
// updates movement, actions, and the full display.
//------------------------------------------------------------------------------
void handleCrop() {
    // Set starting position and direction

    ssd1306_Fill(Black);
	cropDisplay();
	ssd1306_CopyBuffer();

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;  // ~30 FPS

    GrowSpeed = 1; //normal

    game.houseUnlocked = 1;

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            // Process input and update state only once per frame
        	ssd1306_Fill(Black);

        	if (refreshBackground){
        		refreshBackground = 0;
        		cropDisplay();
        		ssd1306_CopyBuffer();
        	}

        	updateButtonFlags();
        	cropPlayerMovement();

            ORBuffer();
        	playerDisplay();

        	cropPlayerAction();

        	ssd1306_UpdateScreen();
            lastFrameTime = now;
        }

        gameLogic();

        HAL_Delay(1);

        // Exit condition: if player goes across bridge
        if (player.coordinates.y < 0) {
            player.inWorld = SHOP;
            player.coordinates.x = 60;
            player.coordinates.y = 48;
            player.direction = UP;
            break;
        }
    }
}
