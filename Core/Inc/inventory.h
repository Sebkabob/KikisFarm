#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "game.h"
#include "main.h"
#include "crop.h"
#include "sprites.h"

const unsigned char* getItemTitle(ItemType itemType);
bool isInventoryEmpty(InventorySlot inventory[]);
void moveInventoryItemsTogether(InventorySlot inventory[]);
int addItemToInventory(InventorySlot inventory[], Item *item, int quantity);
int removeItemFromInventory(InventorySlot inventory[], ItemType itemType, int quantity);
int hasItemInInventory(InventorySlot inventory[], ItemType itemType);
void drawInventoryIcons(int xMov, int yMov);
void drawItemInfo(int itemSelect);
void drawCanPlant(int itemSelect);
int showInventory(int plantSeed);

#endif /* INC_INVENTORY_H_ */
