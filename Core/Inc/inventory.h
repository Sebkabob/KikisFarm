#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "main.h"
#include "game.h"
#include "crop.h"
#include "sprites.h"

int addItemToInventory(InventorySlot inventory[], Item *item, int quantity);
int removeItemFromInventory(InventorySlot inventory[], ItemType itemType, int quantity);
int hasItemInInventory(InventorySlot inventory[], ItemType itemType);
int showInventory(int plantSeed);

#endif /* INC_INVENTORY_H_ */
