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

void orchardDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, OrchardWorldSprite, 128, 64, White);

    char coordString[20];
    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(coordString, Font_6x8, White);
}

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
