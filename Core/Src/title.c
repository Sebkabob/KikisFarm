#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "title.h"

void gameIntro(){
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("By 2097, Earth's farms were fully ruled by AI.", 100, 5, 1);
	textSpeaking("No longer did humans toil in the fields.", 100, 5, 1);
	textSpeaking("Food was plentiful. Life was easy.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("One day, a deadly blight spread across the land.", 100, 5, 1);
	textSpeaking("Crops withered. Harvests failed.", 100, 5, 1);
	textSpeaking("AI was helpless. It had no solution.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("Earth had lost all its farmers.", 100, 5, 1);
	textSpeaking("There was no one left to fix this.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("But a dying breed still remained...", 100, 5, 1);
	textSpeaking("Farming engineers rose from the ashes.", 100, 5, 1);
	textSpeaking("They built new robots unlike any before.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("These machines could till, plant, and harvest.", 100, 5, 1);
	textSpeaking("But AI could no longer be trusted.", 100, 5, 1);
	textSpeaking("Blight-resistant crops needed real oversight.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("Only humans could analyze the soil.", 100, 5, 1);
	textSpeaking("Only humans could adapt in real time.", 100, 5, 1);
	textSpeaking("The robots became tools, not rulers.", 100, 5, 1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	textSpeaking("You are one of the last farmers.", 100, 5, 1);
	textSpeaking("The survival of humanity is in your hands.", 100, 5, 1);
	textSpeaking("Start farming. You have no choice.", 100, 5, 1);

	ssd1306_Fill(Black);
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
