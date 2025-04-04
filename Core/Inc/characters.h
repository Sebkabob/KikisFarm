/*
 * characters.h
 *
 *  Created on: Apr 3, 2025
 *      Author: sebfo
 */

#ifndef INC_CHARACTERS
#define INC_CHARACTERS

#define MAX_HISTORY 30

#define CAT_LOVE_THRESHOLD 200

// Player history and cat following functions
void resetPlayerHistory(void);
void updatePlayerHistory(void);
void catDisplay(void);
void playerDisplay(void);
void playerErase(void);

#endif /* INC_CHARACTERS_H_ */
