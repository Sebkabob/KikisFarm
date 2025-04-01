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

void orchardDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, OrchardWorldSprite, 128, 64, White);

    drawTreeSpot();

    char coordString[20];
    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(coordString, Font_6x8, White);
}

void orchardPlayerAction(){

    // Button A:
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        if (nearXXX()) textSpeaking("testing breaker 9 9", 150, 8, 1);
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
