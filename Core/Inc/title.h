
#ifndef TITLE_H_
#define TITLE_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "sound.h"
#include "ssd1306_fonts.h"
#include "sprites.h"
#include "game.h"

void handleTitle();
void gameIntro(void);
void initGame(void);
void cutToDark(int duration);

#endif /* INC_TITLE_H_ */
