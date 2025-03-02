#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "title.h"

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
        	break;
        }
        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0 && menuSelect == 2) {
        	initGame();
        	pushEEPROM();
        	buzzer(800,25);
        	buzzer(900,25);
        	//are you sure you want to overwrite?
        	for (int i = 0; i < 3; i++){
        		ssd1306_DrawRectangle(42, 36, 86, 44, White);
        		ssd1306_UpdateScreen();
        		buzzer(800,25);
        		HAL_Delay(100);
        		ssd1306_DrawRectangle(42, 36, 86, 44, Black);
        		ssd1306_UpdateScreen();
        		HAL_Delay(100);
        	}
        	cutToDark(75);
        	//gameIntro();
        	player.inWorld = CROP;
        	break;
        }

    }
}

#endif
