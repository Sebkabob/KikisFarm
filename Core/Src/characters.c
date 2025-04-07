/*
 * characters.c
 *
 *  Created on: Apr 3, 2025
 *      Author: sebfo
 */

#include "main.h"
#include "game.h"
#include "crop.h"
#include "sound.h"
#include "sprites.h"
#include "NimaLTD.I-CUBE-EE24_conf.h"
#include "ee24.h"
#include "characters.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
    int direction;
} PositionHistory;

PositionHistory playerHistory[MAX_HISTORY];
int historyIndex = 0;

//------------------------------------------------------------------------------
// Reset the history of the players movements.
//------------------------------------------------------------------------------
void resetPlayerHistory() {
    for (int i = 0; i < MAX_HISTORY; i++) {
        playerHistory[i].x = player.coordinates.x;
        playerHistory[i].y = player.coordinates.y;
        playerHistory[i].direction = player.direction;
    }
    historyIndex = 0;
}

//------------------------------------------------------------------------------
// Remembers where the player walked for the cat to follow.
//------------------------------------------------------------------------------
void updatePlayerHistory() {
    static int lastX = -1;
    static int lastY = -1;
    static World lastWorld;
    static int initialized = 0;
    if (!initialized) {
        lastWorld = player.inWorld;
        initialized = 1;
    }
    if (player.inWorld != lastWorld) {
        lastWorld = player.inWorld;
        resetPlayerHistory();
    }
    if (player.coordinates.x == lastX && player.coordinates.y == lastY) {
        return;
    }
    lastX = player.coordinates.x;
    lastY = player.coordinates.y;
    playerHistory[historyIndex].x = player.coordinates.x;
    playerHistory[historyIndex].y = player.coordinates.y;
    playerHistory[historyIndex].direction = player.direction;
    historyIndex = (historyIndex + 1) % MAX_HISTORY;
}

//------------------------------------------------------------------------------
// Displays the cat in its correct orientation.
//------------------------------------------------------------------------------
void catDisplay() {
	if (cat.sit == 1){
		cat.direction = DOWN;
	}
	else if (cat.love >= CAT_LOVE_THRESHOLD) {
        cat.inWorld = player.inWorld;
        int historyIndexDelayed = (historyIndex + 1) % MAX_HISTORY;
        PositionHistory playerAtDelayed = playerHistory[historyIndexDelayed];
        cat.coordinates.x = playerAtDelayed.x;
        cat.coordinates.y = playerAtDelayed.y;
        cat.direction = playerAtDelayed.direction;
    }
	else {
		cat.direction = DOWN;
	}
    if (cat.inWorld != player.inWorld){
    	return;
    }
    const unsigned char *sprite;
    switch (cat.direction) {
        case DOWN:  sprite = CatDown;  break;
        case UP:    sprite = CatUp;    break;
        case LEFT:  sprite = CatLeft;  break;
        case RIGHT: sprite = CatRight; break;
        default:    sprite = CatDown;  break;
    }
    ssd1306_DrawBitmap(cat.coordinates.x, cat.coordinates.y + 3, sprite, 9, 8, White);
}

//------------------------------------------------------------------------------
// Displays the character in its correct orientation.
//------------------------------------------------------------------------------
void playerDisplay(){
    const unsigned char *sprite;
    switch(player.direction) {
        case DOWN:  sprite = KikiDownSprite;  break;
        case UP:    sprite = KikiUpSprite;    break;
        case LEFT:  sprite = KikiLeftSprite;  break;
        case RIGHT: sprite = KikiRightSprite; break;
        default:    sprite = KikiDownSprite;  break;
    }
    ssd1306_FillRectangle(player.coordinates.x + 2, player.coordinates.y + 2,
                          player.coordinates.x + 5, player.coordinates.y + 5, Black);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, sprite, 9, 11, White);
    updatePlayerHistory();
}
