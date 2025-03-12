#ifndef CROP_HOUSE_H_
#define CROP_HOUSE_H_

#include "inventory.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "crop_house.h"
#include <stdbool.h>

void cropHouseDisplay(void);
bool cropHouseObstacle(int x, int y);
void lightSwitch(void);
void cropHouseIntro(void);
void cropHousePlayerMovement(void);
void cropHousePlayerAction(void);
void handleCropHouse(void);

#endif /* INC_CROP_HOUSE_H_ */
