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
    int direction; // UP, DOWN, LEFT, RIGHT
} PositionHistory;

PositionHistory playerHistory[MAX_HISTORY];
int historyIndex = 0;

void resetPlayerHistory() {
    // Reset the entire history so that all entries match the player's current state.
    for (int i = 0; i < MAX_HISTORY; i++) {
        playerHistory[i].x = player.coordinates.x;
        playerHistory[i].y = player.coordinates.y;
        playerHistory[i].direction = player.direction;
    }
    historyIndex = 0;
}

void updatePlayerHistory() {
    // Use static variables to store the last recorded position and world.
    static int lastX = -1;
    static int lastY = -1;
    static World lastWorld; // Uninitialized by default.
    static int initialized = 0;

    // Initialize lastWorld on the first call.
    if (!initialized) {
        lastWorld = player.inWorld;
        initialized = 1;
    }

    // If the player has changed worlds, reset the history.
    if (player.inWorld != lastWorld) {
        lastWorld = player.inWorld;
        resetPlayerHistory();
    }

    // Only update history if the player's position has actually changed.
    if (player.coordinates.x == lastX && player.coordinates.y == lastY) {
        return;  // No movement, so no update.
    }

    // Update the last known position.
    lastX = player.coordinates.x;
    lastY = player.coordinates.y;

    // Store the player's current position and direction in the history.
    playerHistory[historyIndex].x = player.coordinates.x;
    playerHistory[historyIndex].y = player.coordinates.y;
    playerHistory[historyIndex].direction = player.direction;

    // Move to the next index (wrap around).
    historyIndex = (historyIndex + 1) % MAX_HISTORY;
}

void catDisplay() {
    // Only update if the cat is following the player.
    if (cat.love >= CAT_LOVE_THRESHOLD) {
        // Update the cat's world to match the player's world.
        cat.inWorld = player.inWorld;

        // Get the player's position and direction from the past
        int historyIndexDelayed = (historyIndex + 1) % MAX_HISTORY;
        PositionHistory playerAtDelayed = playerHistory[historyIndexDelayed];

        // Update the cat's position and direction from the player's history.
        cat.coordinates.x = playerAtDelayed.x;
        cat.coordinates.y = playerAtDelayed.y;
        cat.direction = playerAtDelayed.direction;
    } else {
        // If the cat is not following (love < CAT_LOVE_THRESHOLD), don't change its world.
        // Force the cat to face down.
        cat.direction = DOWN;
    }

    if (cat.inWorld != player.inWorld){
    	return;
    }

    // Choose the appropriate sprite based on the cat's direction.
    const unsigned char *sprite;
    switch (cat.direction) {
        case DOWN:  sprite = CatDown;  break;
        case UP:    sprite = CatUp;    break;
        case LEFT:  sprite = CatLeft;  break;
        case RIGHT: sprite = CatRight; break;
        default:    sprite = CatDown;  break;
    }

    // Draw the cat sprite at its current coordinates.
    ssd1306_DrawBitmap(cat.coordinates.x, cat.coordinates.y + 3, sprite, 9, 8, White);
}


void playerDisplay(){
    // Choose the appropriate player sprite.
    const unsigned char *sprite;
    switch(player.direction) {
        case DOWN:  sprite = KikiDownSprite;  break;
        case UP:    sprite = KikiUpSprite;    break;
        case LEFT:  sprite = KikiLeftSprite;  break;
        case RIGHT: sprite = KikiRightSprite; break;
        default:    sprite = KikiDownSprite;  break;
    }

    // Draw the player sprite.
    ssd1306_FillRectangle(player.coordinates.x + 2, player.coordinates.y + 2,
                            player.coordinates.x + 5, player.coordinates.y + 5, Black);
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, sprite, 9, 11, White);

    // Update the player's history after displaying the player.
    updatePlayerHistory();
}


void playerErase(){
    // Choose the appropriate player sprite.
    const unsigned char *sprite;
    switch(player.direction) {
        case DOWN:  sprite = KikiDownSprite;  break;
        case UP:    sprite = KikiUpSprite;    break;
        case LEFT:  sprite = KikiLeftSprite;  break;
        case RIGHT: sprite = KikiRightSprite; break;
        default:    sprite = KikiDownSprite;  break;
    }

    // Draw the player sprite.
    ssd1306_DrawBitmap(player.coordinates.x, player.coordinates.y, sprite, 9, 11, Black);
}
