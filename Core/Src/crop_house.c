#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "crop_house.h"
#include <stdbool.h>

void cropHouseDisplay(){
	//if(game.cropHouseLights) ssd1306_DrawBitmap(0, 0, sprite, 128, 64, White);
}

bool cropHouseObstacle(int x, int y){
	return 0;
}

void lightSwitch(){
//	if (player in location){
//		game.cropHouseIntro = 0;
//		textSpeaking("oh here it is", 150, 8, 1);
//	}
}

void cropHouseIntro(){
	textSpeaking("its so dark in here", 150, 8, 1);
	textSpeaking("(maybe there's a    light switch?)", 150, 8, 1);
}

void cropHousePlayerMovement(){
    uint8_t step = 1;  // Fixed movement step

    // UP button
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // DOWN button
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // LEFT button
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }

    // RIGHT button
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}

void cropHousePlayerAction(){

    // Button A:
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
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

void handleCropHouse() {
    // Set starting position and direction

    ssd1306_Fill(Black);
    cropHouseDisplay();
	ssd1306_CopyBuffer();

	playerDisplay();
	ssd1306_UpdateScreen();

	if (game.cropHouseIntro) cropHouseIntro();

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = 20;  // ~30 FPS

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            // Process input and update state only once per frame
        	ssd1306_Fill(Black);

        	if (refreshBackground){
        		refreshBackground = 0;
        		cropHouseDisplay();
        		ssd1306_CopyBuffer();
        	}

        	updateButtonFlags();
        	cropHousePlayerMovement();

            ORBuffer();
        	playerDisplay();

        	cropHousePlayerAction();

        	ssd1306_UpdateScreen();
            lastFrameTime = now;
        }

        gameLogic();

        HAL_Delay(1);

        if (worldBreak) {
        	worldBreak = 0;
        	break;
        }

        // Exit condition: if player goes across bridge
        if (player.coordinates.y >= 53 &&
           (player.coordinates.x + 7 > 60) &&
           (player.coordinates.x < 60 + 10)) {
            player.inWorld = CROP;
            player.coordinates.x = 12;
            player.coordinates.y = 17;
            player.direction = DOWN;
            break;
        }

    }
}
