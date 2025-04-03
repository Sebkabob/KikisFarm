/*
 * orchard.h
 *
 *  Created on: Apr 1, 2025
 *      Author: sebfo
 */

#ifndef INC_ORCHARD
#define INC_ORCHARD

#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define treeSpotXc1 1
#define treeSpotXc2 19
#define treeSpotXc3 37
#define treeSpotXc4 55
#define treeSpotXc5 73
#define treeSpotXc6 91

#define treeSpotYr1	22
#define treeSpotYr2 29

extern uint32_t treePlantTimes[6]; // Declare globally so other files can use it

void orchardDisplay(void);
bool orchardObstacle(int x, int y);
bool nearXXX(void);
void orchardPlayerMovement(void);
void orchardPlayerAction(void);
void handleOrchard(void);


#endif /* INC_ORCHARD_H_ */
