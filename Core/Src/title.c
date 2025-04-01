#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "title.h"

void gameIntro(){
    int voice1 = 400;  // Voice for character 1
    int voice2 = 120;  // Voice for character 2]
    int speed = 7;

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    textSpeakingFullScreen("oh good you're awake, "
    		"please get back to farming right away!"
    		" You've been out for hours again...", voice2, speed, 1);

    textSpeakingFullScreen("you really gotta stop doing this, "
    		"I'm getting worried about you!", voice2, speed, 1);

    textSpeakingFullScreen("anyways, in case you don't remember,"
    		" you're here to farm!"
    		" I gave you some seeds to plant", voice2, speed, 1);

    textSpeakingFullScreen("press B to check your inventory, "
    		"and stand over an empty crop spot"
    		" and press A to plant seeds", voice2, speed, 1);

    textSpeakingFullScreen("crops grow within a few seconds "
    		"so don't be impatient! Press A to harvest a crop "
    		"once it has grown.", voice2, speed, 1);

    textSpeakingFullScreen("you can sell crops and buy new ones"
    		" when you level up and get some money.", voice2, speed, 1);

    textSpeakingFullScreen("hold B while standing over an existing "
    		"crop to destroy it, "
    		"and try and get back in your house alright?", voice2, speed, 1);

    textSpeakingFullScreen("anyways, take care of yourself, "
    		"please don't do that again, "
    		"I feel like you're not the same anymore...", voice2, speed, 1);
}


int areYouSureMenu(){
    int menuSelect = 1;  // Default: no selected

    // Draw the static text once.
    ssd1306_Fill(Black);
    ssd1306_SetCursor(22, 4);
    ssd1306_WriteString("WARNING!", Font_11x18, White);
    ssd1306_SetCursor(18, 24);
    ssd1306_WriteString("all game data", Font_7x10, White);
    ssd1306_SetCursor(22, 36);
    ssd1306_WriteString("will be lost", Font_7x10, White);
    ssd1306_SetCursor(20, 50);
    ssd1306_WriteString("yes", Font_7x10, White);
    ssd1306_SetCursor(94, 50);
    ssd1306_WriteString("no", Font_7x10, White);

    // Draw initial selection rectangle around "yes"
    ssd1306_DrawRectangle(92, 48, 109, 62, White);
    ssd1306_UpdateScreen();

    sound(areYouSure);

    while (1){
        HAL_Delay(10); // Small delay to prevent overwhelming polling

        // RIGHT button pressed: select "no" (menuSelect = 1)
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
            sound(menuNav);
            if (menuSelect != 1) {
                // Erase the rectangle around "yes"
                ssd1306_DrawRectangle(18, 48, 42, 62, Black);
                // Draw rectangle around "no"
                ssd1306_DrawRectangle(92, 48, 109, 62, White);
                menuSelect = 1;
                ssd1306_UpdateScreen();
            }
            // Wait for the button to be released
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
                HAL_Delay(10);
            }
        }

        // LEFT button pressed: select "yes" (menuSelect = 2)
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
        	sound(menuNav);
            if (menuSelect != 2) {
                // Erase the rectangle around "no"
                ssd1306_DrawRectangle(92, 48, 109, 62, Black);
                // Draw rectangle around "yes"
                ssd1306_DrawRectangle(18, 48, 42, 62, White);
                menuSelect = 2;
                ssd1306_UpdateScreen();
            }
            // Wait for the button to be released
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0) {
                HAL_Delay(10);
            }
        }

        // A button: confirm selection.
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
        	sound(deleteSave);
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

            if (menuSelect == 2) {
                return 1;  // "yes" selected
            } else {
                return 0;  // "no" selected
            }
        }

        // B button: immediately return 0.
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
        	sound(buttonPress);
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            return 0;
        }
    }
}

void controlsTitle(){
	ssd1306_Fill(Black);        // Clear the screen
	ssd1306_DrawBitmap(0, 0, ControlsTitle, 128, 64, White);
	ssd1306_UpdateScreen();
	HAL_Delay(250);
	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1);
}

#if defined(CPP)
void handleTitle() {

	gameStartup();

	int menuSelect = 1;
	ssd1306_Fill(Black);        // Clear the screen
	ssd1306_DrawBitmap(0, 0, TitleWorldSprite, 128, 64, White);
	ssd1306_DrawRectangle(42, 27, 86, 35, White);
	ssd1306_UpdateScreen();
	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
    while(1){
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
        	menuSelect = 1;
        	sound(menuNav);
        	ssd1306_DrawRectangle(42, 27, 86, 35, White);
        	ssd1306_UpdateScreen();
        	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
        	while(HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
        	menuSelect = 2;
        	sound(menuNav);
        	ssd1306_DrawRectangle(42, 36, 86, 44, White);
        	ssd1306_UpdateScreen();
        	ssd1306_DrawRectangle(42, 36, 86, 44, Black);
        	while(HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuSelect == 1) {
        	sound(playGame);
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        	for (int i = 0; i < 3; i++){
            	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
            	ssd1306_UpdateScreen();
            	HAL_Delay(100);
            	ssd1306_DrawRectangle(42, 27, 86, 35, White);
            	ssd1306_UpdateScreen();
            	HAL_Delay(100);
        	}
        	//player.inWorld = CROP;
        	return;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuSelect == 2) {
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        	int areYouSure = areYouSureMenu();
        	if(areYouSure){
            	cutToDark(75);
            	controlsTitle();
            	gameIntro();
            	initGame();
            	pushEEPROM();
            	return;
        	} else {
        		return;
        	}
        }

    }
}

#endif
