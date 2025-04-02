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
        uint16_t color = (treeTiles[0].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc6, treeSpotYr1, TreeSpot, 23, 7, color); // Spot 1
    }
    // Spot 2 -> corresponds to treeTiles[1]
    if (player.soilSpots > 11) {
        uint16_t color = (treeTiles[1].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc5, treeSpotYr2, TreeSpot, 23, 7, color); // Spot 2
    }
    // Spot 3 -> corresponds to treeTiles[2]
    if (player.soilSpots > 12) {
        uint16_t color = (treeTiles[2].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc4, treeSpotYr1, TreeSpot, 23, 7, color); // Spot 3
    }
    // Spot 4 -> corresponds to treeTiles[3]
    if (player.soilSpots > 13) {
        uint16_t color = (treeTiles[3].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc3, treeSpotYr2, TreeSpot, 23, 7, color); // Spot 4
    }
    // Spot 5 -> corresponds to treeTiles[4]
    if (player.soilSpots > 14) {
        uint16_t color = (treeTiles[4].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc2, treeSpotYr1, TreeSpot, 23, 7, color); // Spot 5
    }
    // Spot 6 -> corresponds to treeTiles[5]
    if (player.soilSpots > 15) {
        uint16_t color = (treeTiles[5].tree.id != NONE) ? Black : White;
        ssd1306_DrawBitmap(treeSpotXc1, treeSpotYr2, TreeSpot, 23, 7, color); // Spot 6
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
    // Loop through all 10 crop spots
    for (int i = 0; i < 10; i++) {
        // Only draw if the tile is tilled and a crop has been planted
        if (treeTiles[i].isTilled && treeTiles[i].tree.id != NONE) {
            int x = 0, y = 0;
            // Determine drawing coordinates based on the crop spot number (i+1)
            switch (i + 1) {
                case 1:  x = treeSpotXc1; y = treeSpotYr2 - 22; break;
                case 2:  x = treeSpotXc2; y = treeSpotYr1 - 22; break;
                case 3:  x = treeSpotXc3; y = treeSpotYr2 - 22; break;
                case 4:  x = treeSpotXc4; y = treeSpotYr1 - 22; break;
                case 5:  x = treeSpotXc5; y = treeSpotYr2 - 22; break;
                case 6:  x = treeSpotXc6; y = treeSpotYr1 - 22; break;
                default: break;
            }

            const unsigned char *spriteToDraw = NULL;
            if (treeTiles[i].grown == 0) {
                // Use getItemPointerFromID() to get the item pointer and then its sprite.
                Item *itemPtr = getItemPointerFromID(treeTiles[i].tree.id);
                if (itemPtr != NULL) {
                    spriteToDraw = itemPtr->cropSprite;
                }
            } else {
                // Crop is not fully grown: use the sprout sprite.
                spriteToDraw = SproutSprite;
            }
            Item *itemPtr = getItemPointerFromID(treeTiles[i].tree.id);
            if (itemPtr != NULL) {
                spriteToDraw = itemPtr->cropSprite;
            }
            ssd1306_DrawBitmap(x, y, spriteToDraw, 17, 26, White);
        }
    }
}

void orchardDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, OrchardWorldSprite, 128, 64, White);

    drawTreeSpot();
    drawTrees();

    int tree = checkIfOnTreeSpot();
        char treeSpot[20];
        sprintf(treeSpot, "%d", tree);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString(treeSpot, Font_6x8, White);


    char coordString[20];
    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);
    ssd1306_SetCursor(50, 0);
    ssd1306_WriteString(coordString, Font_6x8, White);
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
                cropPlantTimes[spot - 1] = HAL_GetTick();

                // Remove one unit of the seed from the player's inventory
                removeItemFromInventory(player.inventory,
                                        player.inventory[slot].item->id,
                                        1);
            }
        }
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
            if (treeTiles[spot - 1].grown == 4) {
                //treeHarvest();
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
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
        buzzer(300, 25);
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
    }
}

void handleOrchard() {
    // Set starting position and direction

    ssd1306_Fill(Black);
    orchardDisplay();
	ssd1306_CopyBuffer();

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
