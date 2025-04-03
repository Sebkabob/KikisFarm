/*
 * orchard.c
 *
 *  Created on: Apr 1, 2025
 *      Author: sebfo
 */
#include "orchard.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "crop_house.h"
#include <stdbool.h>

#define NEAR_THRESHOLD 1

extern TreeTile treeTiles[];

Item getGrownSapling(ItemType saplingId);

bool orchardObstacle(int x, int y) {

	//Obstacle 1: Water
    if (x < 68 || x > 120 || y < 24 || y > 53)
        return false;
    double y_line = 53 - ((x - 68) * 29.0 / 52.0);
    if (y >= y_line)
        return true;
    else
        return false;
}

bool nearXXX() {
    int left = 12, right = 38, top = 22, bottom = 36;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}


//------------------------------------------------------------------------------
// Checks if the player is on one of the tree spots.
// Returns a number 1-10 if on a spot, 0 otherwise.
//------------------------------------------------------------------------------
int checkIfOnTreeSpot(void) {
    // Check if the player is on a tree spot and if the spot is unlocked
    if (player.coordinates.x >= treeSpotXc1 - 3 && player.coordinates.x < treeSpotXc1 + 18 &&
        player.coordinates.y >= treeSpotYr2 - 10 && player.coordinates.y <= treeSpotYr2 - 6) {
        if (player.soilSpots > 15) // Spot 6 is unlocked if soilSpots > 15
            return 6;
    }
    if (player.coordinates.x >= treeSpotXc2 - 3 && player.coordinates.x < treeSpotXc2 + 18 &&
        player.coordinates.y >= treeSpotYr1 - 10 && player.coordinates.y <= treeSpotYr1 - 6) {
        if (player.soilSpots > 14) // Spot 5 is unlocked if soilSpots > 14
            return 5;
    }
    if (player.coordinates.x >= treeSpotXc3 - 3 && player.coordinates.x < treeSpotXc3 + 18 &&
        player.coordinates.y >= treeSpotYr2 - 10 && player.coordinates.y <= treeSpotYr2 - 6) {
        if (player.soilSpots > 13) // Spot 4 is unlocked if soilSpots > 13
            return 4;
    }
    if (player.coordinates.x >= treeSpotXc4 - 3 && player.coordinates.x < treeSpotXc4 + 18 &&
        player.coordinates.y >= treeSpotYr1 - 10 && player.coordinates.y <= treeSpotYr1 - 6) {
        if (player.soilSpots > 12) // Spot 3 is unlocked if soilSpots > 12
            return 3;
    }
    if (player.coordinates.x >= treeSpotXc5 - 3 && player.coordinates.x < treeSpotXc5 + 18 &&
        player.coordinates.y >= treeSpotYr2 - 10 && player.coordinates.y <= treeSpotYr2 - 6) {
        if (player.soilSpots > 11) // Spot 2 is unlocked if soilSpots > 11
            return 2;
    }
    if (player.coordinates.x >= treeSpotXc6 - 3 && player.coordinates.x < treeSpotXc6 + 18 &&
        player.coordinates.y >= treeSpotYr1 - 10 && player.coordinates.y <= treeSpotYr1 - 6) {
        if (player.soilSpots > 10) // Spot 1 is unlocked if soilSpots > 10
            return 1;
    }

    return 0; // Not on a sapling spot or the spot is not unlocked
}

//------------------------------------------------------------------------------
// Draws soil based on the number of tilled spots.
//------------------------------------------------------------------------------
void drawTreeSpot(void) {
    // Spot 1 -> corresponds to treeTiles[0]
    if (player.soilSpots > 10) {
        ssd1306_DrawBitmap(treeSpotXc6, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 1
    }
    // Spot 2 -> corresponds to treeTiles[1]
    if (player.soilSpots > 11) {
        ssd1306_DrawBitmap(treeSpotXc5, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 2
    }
    // Spot 3 -> corresponds to treeTiles[2]
    if (player.soilSpots > 12) {
        ssd1306_DrawBitmap(treeSpotXc4, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 3
    }
    // Spot 4 -> corresponds to treeTiles[3]
    if (player.soilSpots > 13) {
        ssd1306_DrawBitmap(treeSpotXc3, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 4
    }
    // Spot 5 -> corresponds to treeTiles[4]
    if (player.soilSpots > 14) {
        ssd1306_DrawBitmap(treeSpotXc2, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 5
    }
    // Spot 6 -> corresponds to treeTiles[5]
    if (player.soilSpots > 15) {
        ssd1306_DrawBitmap(treeSpotXc1, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 6
    }
}

void orchardPlayerMovement(){
    uint8_t step = 1;  // Fixed movement step

    // UP button
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE + 8 && !orchardObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // DOWN button
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE && !orchardObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // LEFT button
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE && !orchardObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }

    // RIGHT button
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE && !orchardObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}

//------------------------------------------------------------------------------
// Draws trees
//------------------------------------------------------------------------------
void drawTrees(void) {
    for (int i = 0; i < 6; i++) {
        if (treeTiles[i].isTilled && treeTiles[i].tree.id != NONE) {
            int x = 0, y = 0;
            // Determine drawing coordinates based on the tree spot number (i+1)
            switch (i + 1) {
                case 1:  x = treeSpotXc6 + 3; y = treeSpotYr1 - 22; break;
                case 2:  x = treeSpotXc5 + 3; y = treeSpotYr2 - 22; break;
                case 3:  x = treeSpotXc4 + 3; y = treeSpotYr1 - 22; break;
                case 4:  x = treeSpotXc3 + 3; y = treeSpotYr2 - 22; break;
                case 5:  x = treeSpotXc2 + 3; y = treeSpotYr1 - 22; break;
                case 6:  x = treeSpotXc1 + 3; y = treeSpotYr2 - 22; break;
                default: break;
            }

            // Draw the sprite based on the growth stage
            switch (treeTiles[i].grown) {
                case 0:		ssd1306_DrawBitmap(x+6, y+24, SproutTreeSprite, 5, 2, White);   break;
                case 1:     ssd1306_DrawBitmap(x+6, y+16, SaplingTreeSprite, 6, 10, White); break;
                case 2: {
                    ssd1306_DrawBitmap(x, y, EraseTreeSprite, 17, 26, Black);
                	ssd1306_DrawBitmap(x, y, EmptyTreeSprite, 17, 26, White);  break;
                }
                case 3: {
                    Item *itemPtr = getItemPointerFromID(treeTiles[i].tree.id);
                    if (itemPtr != NULL && itemPtr->cropSprite != NULL) {
                        ssd1306_DrawBitmap(x, y, EraseTreeSprite, 17, 26, Black);
                        ssd1306_DrawBitmap(x, y, itemPtr->cropSprite, 17, 26, White);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void drawBlockingTrees(){
    for (int i = 0; i < 6; i++) {
        if (!treeTiles[i].isTilled || treeTiles[i].tree.id == NONE)
            continue;

        int spot = i + 1;
        int x = 0, y = 0;
        bool shouldDraw = false;

        // Even spots: 2, 4, 6 use treeSpotYr2.
        if (spot % 2 == 0) {
            if (player.coordinates.y < 22) {
                shouldDraw = true;
                switch (spot) {
                    case 2:  x = treeSpotXc5 + 3; y = treeSpotYr2 - 22; break;
                    case 4:  x = treeSpotXc3 + 3; y = treeSpotYr2 - 22; break;
                    case 6:  x = treeSpotXc1 + 3; y = treeSpotYr2 - 22; break;
                    default: break;
                }
            }
        }
        // Odd spots: 1, 3, 5 use treeSpotYr1.
        else {
            if (player.coordinates.y < 15) {
                shouldDraw = true;
                switch (spot) {
                    case 1:  x = treeSpotXc6 + 3; y = treeSpotYr1 - 22; break;
                    case 3:  x = treeSpotXc4 + 3; y = treeSpotYr1 - 22; break;
                    case 5:  x = treeSpotXc2 + 3; y = treeSpotYr1 - 22; break;
                    default: break;
                }
            }
        }

        if (shouldDraw) {
            // Draw the sprite based on the growth stage (same logic as in drawTrees)
            switch (treeTiles[i].grown) {
                case 0:
                    ssd1306_DrawBitmap(x + 6, y + 24, SproutTreeSprite, 5, 2, White);
                    break;
                case 1:
                    ssd1306_DrawBitmap(x + 6, y + 16, SaplingTreeSprite, 6, 10, White);
                    break;
                case 2:
                    ssd1306_DrawBitmap(x, y, EraseTreeSprite, 17, 26, Black);
                    ssd1306_DrawBitmap(x, y, EmptyTreeSprite, 17, 26, White);
                    break;
                case 3: {
                    Item *itemPtr = getItemPointerFromID(treeTiles[i].tree.id);
                    if (itemPtr != NULL && itemPtr->cropSprite != NULL) {
                        ssd1306_DrawBitmap(x, y, EraseTreeSprite, 17, 26, Black);
                        ssd1306_DrawBitmap(x, y, itemPtr->cropSprite, 17, 26, White);
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}


void animateWater() {
    static const uint8_t* frames[] = {
        WaterAnimation1,
        WaterAnimation2,
        WaterAnimation3,
        WaterAnimation4,
        WaterAnimation5
    };

    static int index = 0;
    static int direction = 1;
    static uint32_t lastFrameSwitch = 0;

    uint32_t now = HAL_GetTick();

    // Always draw the current frame (no matter how much time has passed)
    ssd1306_DrawBitmap(128 - 46, 64 - 28, frames[index], 46, 28, White);

    // Only update the frame index every 400ms
    if (now - lastFrameSwitch >= 400) {
        lastFrameSwitch = now;

        // Update index
        index += direction;

        // Reverse direction at the ends
        if (index == 4) direction = -1;  // reached frame 5 (index 4)
        else if (index == 0) direction = 1;  // reached frame 1 (index 0)
    }
}




void orchardDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, OrchardWorldSprite, 128, 64, White);
    animateWater();

    drawTreeSpot();
    drawTrees();

//    char coordString[20];
//    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);
//    ssd1306_SetCursor(128 - (6*9), 64-8);
//    ssd1306_WriteString(coordString, Font_6x8, White);
}

void treePlant(){
    int spot = checkIfOnTreeSpot();  // Returns a number 1–10 if on a valid sapling spot.
    // If no grown sapling, allow planting if the spot is empty
    if (spot != 0 && treeTiles[spot - 1].tree.id == NONE) {
        sound(inventoryOpen);

        // Let the player select a seed from the inventory
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
        }

        ItemType saplingId = showInventory(2);
        // If a valid seed id was returned, find the corresponding inventory slot
        if (saplingId != NONE) {
            int slot = -1;
            for (int i = 0; i < 9; i++) {
                if (player.inventory[i].item != NULL &&
                    player.inventory[i].item->id == saplingId) {
                    slot = i;
                    break;
                }
            }
            if (slot != -1) {
                // Use the helper function to convert the seed to its grown sapling.
                Item sapling = getGrownSapling(saplingId);
                if (sapling.id == NONE) {
                    // Handle error if the conversion failed.
                    return;
                }

                // Plant the corresponding sapling on the sapling tile
                treeTiles[spot - 1].tree = sapling;
                treeTiles[spot - 1].grown = 0;
                treeTiles[spot - 1].isTilled = true;

                // Record the time when the sapling was planted
                treePlantTimes[spot - 1] = HAL_GetTick();

                // Remove one unit of the seed from the player's inventory
                removeItemFromInventory(player.inventory,
                                        player.inventory[slot].item->id,
                                        1);
            }
        }
    }
}

void treeHarvest(){
    int spot = checkIfOnTreeSpot();  // Returns a number 1–10 if on a valid crop spot.

    if (spot != 0 && treeTiles[spot - 1].grown == 3) {
        // Play harvesting tones
        Item *harvestedTree = NULL;
        switch (treeTiles[spot - 1].tree.id) {
            case APPLE:   harvestedTree = &apple;   break;
            case ORANGE:  harvestedTree = &orange;  break;
            case BANANA:  harvestedTree = &banana;  break;
            case CHERRY:  harvestedTree = &cherry;  break;
            case MONEY:   harvestedTree = &money;   break;
            default:
                // If no matching crop, handle the error.
                return;
        }

        // For non-money trees, add the harvested item to the inventory.
        if (!addItemToInventory(player.inventory, harvestedTree, harvestedTree->levelUnlock)) {
            sound(inventoryFull);
            return;
        }

        sound(harvest);

        // Increase player's XP and reset tree state.
        player.xp += treeTiles[spot - 1].tree.xp;
        treeTiles[spot - 1].grown = 2;
        treePlantTimes[spot - 1] = HAL_GetTick();

        ssd1306_UpdateScreen();
    }
}

void orchardPlayerAction(){

    // Button A:
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

        int spot = checkIfOnTreeSpot();  // Returns a number 1–6 if on a valid tree spot.
        if (spot != 0) {
            // If a grown sapling exists, harvest it.
            if (treeTiles[spot - 1].grown == 3) {
                treeHarvest();
            }
            // Otherwise, if the spot is empty, plant a seed.
            else if (treeTiles[spot - 1].tree.id == NONE) {
                treePlant();
            }
        }

        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    }

    // Button B:
    if (B_Button_Flag) {
    	B_Button_Flag = 0;
    	refreshBackground = 1;
        uint32_t startTime = HAL_GetTick();
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
            if (HAL_GetTick() - startTime >= 1500) {
                // Held for 1.5 seconds: destroy the crop.
                int spot = checkIfOnTreeSpot();
                if (spot != 0 && treeTiles[spot - 1].tree.id != NONE) {
                    treeTiles[spot - 1].tree.id = NONE;
                    treeTiles[spot - 1].grown = 0;
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
        int p = gameMenu();  // Assume gameMenu() returns a flag.
        if (p)
            leaveWorld = 1;
    }

    // START button:
    if (START_Button_Flag) {
    	START_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        theMap();
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
    }
}

void handleOrchard() {
    // Set starting position and direction

    ssd1306_Fill(Black);
    orchardDisplay();
	playerDisplay();
	ssd1306_UpdateScreen();

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;  // ~30 FPS

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            // Process input and update state only once per frame
        	ssd1306_Fill(Black);

        	orchardDisplay();

        	updateButtonFlags();
        	orchardPlayerMovement();

        	playerDisplay();
        	drawBlockingTrees();

        	orchardPlayerAction();

        	ssd1306_UpdateScreen();
            lastFrameTime = now;
        }

        gameLogic();

        HAL_Delay(1);

        if (worldBreak) {
        	worldBreak = 0;
        	break;
        }

        // Exit condition: if player exits the orchard
        if (player.coordinates.x < 1) {
            player.inWorld = CROP;
            player.coordinates.x = 117;
            break;
        }

    }
}
