#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "crop_house.h"
#include <stdbool.h>

#define NEAR_THRESHOLD 1

void cropHouseDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, TheHouse, 128, 64, White);

    // Prepare the player's coordinates string
    char coordString[20];
    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);

    // Set the cursor position to an area that doesn't interfere with the main graphic.
    // Here, we choose a y value of 56 (near the bottom) so that the text is visible.
    ssd1306_SetCursor(0, 0);

    // Write the coordinate string on the display using a chosen font (e.g., Font_6x8)
    //ssd1306_WriteString(coordString, Font_6x8, White);
}

bool cropHouseObstacle(int x, int y) {
    // Obstacle 1: Arcade machine.
    if (x >= 62 && x <= 79 && y >= 0 && y <= 18)
        return true;

    // Obstacle 2: Bookshelf
    if (x >= 18 && x <= 57 && y >= 0 && y <= 18)
        return true;

    // Obstacle 3: Table
    if (x >= 12 && x <= 37 && y >= 22 && y <= 36)
        return true;

    // Obstacle 4: Stairwell
    if (((x >= 0 && x <= 30) && (y >= 42 && y <= 46)) || ((y >= 51 && y <= 53) && (x >= 0 && x <= 26)))
        return true;

    return false;
}

bool nearTable() {
    int left = 12, right = 38, top = 22, bottom = 36;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearBookshelf() {
    int left = 18, right = 57, top = 0, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearArcadeMachine() {
    int left = 61, right = 80, top = 0, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearRasins() {
    int left = 102, right = 107, top = 26, bottom = 32;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearPee() {
    int left = 97, right = 107, top = 44, bottom = 52;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearDoor() {
    int left = 89, right = 97, top = 17, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}


void cropHouseIntro(){
	textSpeaking("its a dump in here", 150, 8, 1);
	textSpeaking("(it smells like     stale bread)", 150, 8, 1);
	textSpeaking("i wonder who used to live here...", 150, 8, 1);
}

void cropHousePlayerMovement(){
    uint8_t step = 1;  // Fixed movement step

    // UP button
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE + 16 && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // DOWN button
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE - 1 && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // LEFT button
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE + 12 && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }

    // RIGHT button
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE - 12 && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}

void cropHousePlayerAction(){

    // Button A:
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        if (nearArcadeMachine()) textSpeaking("a broken arcade     machine, wonder if  it can be fixed...", 150, 8, 1);
        if (nearBookshelf()) textSpeaking("must be books of    lore, too bad the   update isnt here...", 150, 8, 1);
        if (nearTable()) textSpeaking("a walnut table...", 150, 8, 1);
        if (nearRasins()) textSpeaking("ew ew ew,           raisins on the floor", 150, 8, 1);
        if (nearPee()) textSpeaking("piss on the floor,  real classy", 150, 8, 1);
        if (nearDoor()) textSpeaking("another locked door, great...", 150, 8, 1);
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

void handleCropHouse() {
    // Set starting position and direction

    ssd1306_Fill(Black);
    cropHouseDisplay();
	ssd1306_CopyBuffer();

	playerDisplay();
	ssd1306_UpdateScreen();

	if (game.cropHouseIntro){
		cropHouseIntro();
		game.cropHouseIntro = 0;
	}

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;  // ~30 FPS

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            // Process input and update state only once per frame
        	ssd1306_Fill(Black);

        	cropHouseDisplay();

        	updateButtonFlags();
        	cropHousePlayerMovement();

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

        // Exit condition: if player exits the house
        if (player.coordinates.y >= 52 &&
           (player.coordinates.x + 5 > 60) &&
           (player.coordinates.x < 60 + 6)) {
            player.inWorld = CROP;
            player.coordinates.x = 12;
            player.coordinates.y = 17;
            player.direction = DOWN;
            break;
        }

    }
}
