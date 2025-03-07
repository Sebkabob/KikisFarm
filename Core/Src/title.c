#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "title.h"

void gameIntro(){
    int voice1 = 600;  // Voice for character 1
    int voice2 = 200;  // Voice for character 2

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    // Character 1: "Whats going on... Where am I?"
    textSpeaking("Whats going on... Where am I?", voice1, 5, 1);

    // Character 2: "You have been transported to planet 43X"
    textSpeaking("You have been transported to planet 43X", voice2, 5, 1);
    // Character 2: "We need you to farm"
    textSpeaking("We need you to farm", voice2, 5, 1);

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    // Character 1: "Farm? What are you talking about"
    textSpeaking("Farm? What are you talking about", voice1, 5, 1);
    // Character 2: "..."
    textSpeaking("...", voice2, 5, 1);

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    // Character 1: "What is going on?? What is happening"
    textSpeaking("What is going on?? What is happening", voice1, 5, 1);
    // Character 2: "You have been transferred to this district, we need you to start farming immediately"
    textSpeaking("You have been transferred to this district, we need you to start farming immediately", voice2, 5, 1);

    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);

    // Character 1: "What?????"
    textSpeaking("What?????", voice1, 5, 1);

    ssd1306_Fill(Black);
}


int areYouSureMenu(){
    int menuSelect = 2;  // Default: yes selected

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
    ssd1306_DrawRectangle(18, 48, 42, 62, White);
    ssd1306_UpdateScreen();

    buzzer(200,300);
    buzzer(140,300);

    while (1){
        HAL_Delay(10); // Small delay to prevent overwhelming polling

        // RIGHT button pressed: select "no" (menuSelect = 1)
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0) {
            buzzer(300,25);
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
            buzzer(300,25);
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
            buzzer(800,50);
            buzzer(900,75);
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

            if (menuSelect == 2) {
                return 1;  // "yes" selected
            } else {
                return 0;  // "no" selected
            }
        }

        // B button: immediately return 0.
        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0) {
            buzzer(800,25);
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
            return 0;
        }
    }
}

void controlsTitle(){
	ssd1306_Fill(Black);        // Clear the screen
	ssd1306_DrawBitmap(0, 0, ControlsTitle, 128, 64, White);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1);
}

#if defined(CPP)
void handleTitle() {
	int menuSelect = 1;
	ssd1306_Fill(Black);        // Clear the screen
	ssd1306_DrawBitmap(0, 0, TitleWorldSprite, 128, 64, White);
	ssd1306_DrawRectangle(42, 27, 86, 35, White);
	ssd1306_UpdateScreen();
	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
    while(1){
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0) {
        	menuSelect = 1;
        	ssd1306_DrawRectangle(42, 27, 86, 35, White);
        	ssd1306_UpdateScreen();
        	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
        	while(HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0) {
        	menuSelect = 2;
        	ssd1306_DrawRectangle(42, 36, 86, 44, White);
        	ssd1306_UpdateScreen();
        	ssd1306_DrawRectangle(42, 36, 86, 44, Black);
        	while(HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuSelect == 1) {
        	buzzer(800,25);
        	buzzer(900,25);
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        	for (int i = 0; i < 3; i++){
            	ssd1306_DrawRectangle(42, 27, 86, 35, Black);
            	ssd1306_UpdateScreen();
            	buzzer(800,25);
            	HAL_Delay(100);
            	ssd1306_DrawRectangle(42, 27, 86, 35, White);
            	ssd1306_UpdateScreen();
            	HAL_Delay(100);
        	}
        	player.inWorld = CROP;
        	return;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuSelect == 2) {
        	while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        	int areYouSure = areYouSureMenu();
        	if(areYouSure){
            	buzzer(800,25);
            	buzzer(900,25);
            	cutToDark(75);
            	//gameIntro();
            	controlsTitle();
            	initGame();
            	pushEEPROM();
            	player.inWorld = CROP;
            	return;
        	} else {
        		return;
        	}
        }

    }
}

#endif
