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
#include "characters.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define NEAR_THRESHOLD 1

extern TreeTile treeTiles[];

Item getGrownSapling(ItemType saplingId);


//------------------------------------------------------------------------------
// Returns true if player is within bounds.
//------------------------------------------------------------------------------
bool orchardObstacle(int x, int y) {
    if (x < 68 || x > 120 || y < 24 || y > 53)
        return false;
    double y_line = 53 - ((x - 68) * 29.0 / 52.0);
    if (y >= y_line)
        return true;
    else
        return false;
}

//------------------------------------------------------------------------------
// Unused Function.
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
int checkIfOnTreeSpot(void) {
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
    if (player.soilSpots > 10) {
        ssd1306_DrawBitmap(treeSpotXc6, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 1
    }
    if (player.soilSpots > 11) {
        ssd1306_DrawBitmap(treeSpotXc5, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 2
    }
    if (player.soilSpots > 12) {
        ssd1306_DrawBitmap(treeSpotXc4, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 3
    }
    if (player.soilSpots > 13) {
        ssd1306_DrawBitmap(treeSpotXc3, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 4
    }
    if (player.soilSpots > 14) {
        ssd1306_DrawBitmap(treeSpotXc2, treeSpotYr1, TreeSpot, 23, 7, White); // Spot 5
    }
    if (player.soilSpots > 15) {
        ssd1306_DrawBitmap(treeSpotXc1, treeSpotYr2, TreeSpot, 23, 7, White); // Spot 6
    }
}

//------------------------------------------------------------------------------
// Handles player movement in the orchard.
//------------------------------------------------------------------------------
void orchardPlayerMovement(){
    uint8_t step = 1;
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE + 8 && !orchardObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE && !orchardObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE && !orchardObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE && !orchardObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}

//------------------------------------------------------------------------------
// Draws trees dependent on their grown state.
//------------------------------------------------------------------------------
void drawTrees(void) {
    for (int i = 0; i < 6; i++) {
        if (treeTiles[i].isTilled && treeTiles[i].tree.id != NONE) {
            int x = 0, y = 0;
            switch (i + 1) {
                case 1:  x = treeSpotXc6 + 3; y = treeSpotYr1 - 22; break;
                case 2:  x = treeSpotXc5 + 3; y = treeSpotYr2 - 22; break;
                case 3:  x = treeSpotXc4 + 3; y = treeSpotYr1 - 22; break;
                case 4:  x = treeSpotXc3 + 3; y = treeSpotYr2 - 22; break;
                case 5:  x = treeSpotXc2 + 3; y = treeSpotYr1 - 22; break;
                case 6:  x = treeSpotXc1 + 3; y = treeSpotYr2 - 22; break;
                default: break;
            }
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

//------------------------------------------------------------------------------
// Redraws trees over the character if they are behind the tree.
//------------------------------------------------------------------------------
void drawBlockingTrees(){
    for (int i = 0; i < 6; i++) {
        if (!treeTiles[i].isTilled || treeTiles[i].tree.id == NONE)
            continue;
        int spot = i + 1;
        int x = 0, y = 0;
        bool shouldDraw = false;
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

//------------------------------------------------------------------------------
// Draws the animated water.
//------------------------------------------------------------------------------
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
    ssd1306_DrawBitmap(128 - 46, 64 - 28, frames[index], 46, 28, White);
    if (now - lastFrameSwitch >= 400) {
        lastFrameSwitch = now;
        index += direction;
        if (index == 4) direction = -1;
        else if (index == 0) direction = 1;
    }
}

//------------------------------------------------------------------------------
// Draws the background of the orchard.
//------------------------------------------------------------------------------
void orchardDisplay(){
    ssd1306_DrawBitmap(0, 0, OrchardWorldSprite, 128, 64, White);
    animateWater();
    drawTreeSpot();
    drawTrees();
}

//------------------------------------------------------------------------------
// Handles the act of planting a tree.
//------------------------------------------------------------------------------
void treePlant(){
    int spot = checkIfOnTreeSpot();
    if (spot != 0 && treeTiles[spot - 1].tree.id == NONE) {
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
        }
        ItemType saplingId = showInventory(2);
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
                Item sapling = getGrownSapling(saplingId);
                if (sapling.id == NONE) {
                    return;
                }
                treeTiles[spot - 1].tree = sapling;
                treeTiles[spot - 1].grown = 0;
                treeTiles[spot - 1].isTilled = true;
                treePlantTimes[spot - 1] = HAL_GetTick();
                removeItemFromInventory(player.inventory,
                                        player.inventory[slot].item->id,
                                        1);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Handles the act of harvesting a tree.
//------------------------------------------------------------------------------
void treeHarvest(){
    int spot = checkIfOnTreeSpot();
    if (spot != 0 && treeTiles[spot - 1].grown == 3) {
        Item *harvestedTree = NULL;
        switch (treeTiles[spot - 1].tree.id) {
            case APPLE:   harvestedTree = &apple;   break;
            case ORANGE:  harvestedTree = &orange;  break;
            case BANANA:  harvestedTree = &banana;  break;
            case CHERRY:  harvestedTree = &cherry;  break;
            case MONEY:   harvestedTree = &money;   break;
            default:
                return;
        }
        if (!addItemToInventory(player.inventory, harvestedTree, harvestedTree->levelUnlock)) {
            sound(inventoryFull);
            return;
        }
        sound(harvest);
        player.xp += treeTiles[spot - 1].tree.xp;
        treeTiles[spot - 1].grown = 2;
        treePlantTimes[spot - 1] = HAL_GetTick();
        ssd1306_UpdateScreen();
    }
}

//------------------------------------------------------------------------------
// Hmmmmm.
//------------------------------------------------------------------------------
void ponderingThought(void) {
    // Generate a random number between 1 and 10.
    int randomNum = (rand() % 10) + 1;

    // Process the random number using a switch statement.
    switch (randomNum) {
        case 1:
        	textSpeaking("Damn.", 150, 8, 1);
            break;
        case 2://///////*/////////////////////####################????????????????????
        	textSpeaking("This game must have been a bitch and a  half to make", 150, 8, 1);
            break;
        case 3://///////*/////////////////////####################????????????????????
        	textSpeaking("I wonder what egg   prices are right now?", 150, 8, 1);
            break;
        case 4://///////*/////////////////////####################????????????????????
        	textSpeaking("This water is so    beautiful...", 150, 8, 1);
            break;
        case 5://///////*/////////////////////####################????????????????????
        	textSpeaking("*nothing at all*", 150, 8, 1);
            break;
        case 6://///////*/////////////////////####################????????????????????
        	textSpeaking("I wonder where the  boat will take me...", 150, 8, 1);
            break;
        case 7://///////*/////////////////////####################????????????????????
        	textSpeaking("I love my mother", 150, 8, 1);
            break;
        case 8://///////*/////////////////////####################????????????????????
        	textSpeaking("Costco pizza is so  good but really     oily", 150, 8, 1);
            break;
        case 9://///////*/////////////////////####################????????????????????
        	textSpeaking("I wonder where seb  is right now?", 150, 8, 1);
        	textSpeaking("I should text him.", 150, 8, 1);
            break;
        case 10:////////*/////////////////////####################????????????????????
        	textSpeaking("HfdSKkvm9(SI89*N@#Nnf#@snFNcWSNkjdnwdnWdWDNIKJWNdnwJCNwjNnwd", 150, 8, 1);
            break;
        default:
            // This should never happen.
        	textSpeaking("damn.", 150, 8, 1);
            break;
    }
}

//------------------------------------------------------------------------------
// Handles button actions.
//------------------------------------------------------------------------------
void orchardPlayerAction(){
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        if (orchardObstacle(player.coordinates.x + 1, player.coordinates.y + 1)){
        	textSpeaking("you ponder and look at the water...", 150, 8, 1);
        	textSpeaking("you think to        yourself:", 150, 8, 1);
        	ponderingThought();
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        }
        int spot = checkIfOnTreeSpot();
        if (spot != 0) {
        	if (cat.sit == 1){
        		petFeed();
        	}
        	if (treeTiles[spot - 1].grown == 3) {
                treeHarvest();
            }
            else if (treeTiles[spot - 1].tree.id == NONE) {
                treePlant();
            }
        }
        else {
            petFeed();
        }
    }
    if (B_Button_Flag) {
    	B_Button_Flag = 0;
    	refreshBackground = 1;
        uint32_t startTime = HAL_GetTick();
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            HAL_Delay(10);
            if (HAL_GetTick() - startTime >= 1500) {
                int spot = checkIfOnTreeSpot();
                if (spot != 0 && treeTiles[spot - 1].tree.id != NONE) {
                    treeTiles[spot - 1].tree.id = NONE;
                    treeTiles[spot - 1].grown = 0;
                    sound(destroy);
                } else if (spot == 0){
                    petSit();
                }
                while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
                return;
            }
        }
        showInventory(0);
    }
    if (SELECT_Button_Flag) {
    	SELECT_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
        statbarShow = 0;
        refreshBackground = 1;
        int p = gameMenu();
        if (p)
            leaveWorld = 1;
    }
    if (START_Button_Flag) {
    	START_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        theMap();
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
    }
}

//------------------------------------------------------------------------------
// Main loop for the orchard world: updates movement, actions, and the full display.
//------------------------------------------------------------------------------
void handleOrchard() {
    ssd1306_Fill(Black);
    orchardDisplay();
    catDisplay();
	playerDisplay();
	ssd1306_UpdateScreen();

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
        	ssd1306_Fill(Black);

        	orchardDisplay();

        	updateButtonFlags();
        	orchardPlayerMovement();

            catDisplay();
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
        if (player.coordinates.x < 1) {
            player.inWorld = CROP;
            player.coordinates.x = 117;
            break;
        }

    }
}
