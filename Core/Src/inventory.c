#include "inventory.h"

//------------------------------------------------------------------------------
// Draws all inventory icons in the grid.
//------------------------------------------------------------------------------
void drawInventoryIcons(int xMov, int yMov){
    // Draw the items in the inventory
    for (int i = 0; i < 9; i++) {
        if (player.inventory[i].item != NULL && player.inventory[i].item->id != NONE) {
            xMov = ((i % 3) * 17);
            yMov = ((i / 3) * 17);
            const unsigned char *spriteToDraw;
            if (player.inventory[i].item->itemSprite != NULL){
                spriteToDraw = player.inventory[i].item->itemSprite;
            	ssd1306_DrawBitmap(8 + xMov, 4 + yMov, spriteToDraw, 14, 14, White);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Draws the title and quantity of a selected item.
//------------------------------------------------------------------------------
void drawItemInfo(int itemSelect){
    if (player.inventory[itemSelect - 1].item != NULL && player.inventory[itemSelect - 1].item->id != NONE) {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
        ssd1306_DrawBitmap(57, 2, player.inventory[itemSelect - 1].item->titleSprite, 65, 24, White);
        char quantityText[10];
        snprintf(quantityText, sizeof(quantityText), "x%02d", player.inventory[itemSelect - 1].quantity);
        ssd1306_SetCursor(82, 25);
        ssd1306_WriteString(quantityText, Font_6x8, White);
    } else {
        ssd1306_FillRectangle(57, 2, 121, 53, Black);
    }
}

//------------------------------------------------------------------------------
// Returns true if entire inventory is empty.
//------------------------------------------------------------------------------
bool isInventoryEmpty(InventorySlot inventory[]) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id != NONE) {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
// Moves items so they are justified to the first grid square.
//------------------------------------------------------------------------------
void moveInventoryItemsTogether(InventorySlot inventory[]) {
    int nextFree = 0;
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id != NONE) {
            if (i != nextFree) {
                inventory[nextFree] = inventory[i];
                inventory[i].item = NULL;
                inventory[i].quantity = 0;
            }
            nextFree++;
        }
    }
}

//------------------------------------------------------------------------------
// Adds items at a desired quantity to the inventory.
//------------------------------------------------------------------------------
int addItemToInventory(InventorySlot inventory[], Item *item, int quantity) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == item->id && inventory[i].quantity < 99) {
            int space = 99 - inventory[i].quantity;
            int add = (quantity > space) ? space : quantity;
            inventory[i].quantity += add;
            quantity -= add;
            if (quantity == 0) return 1;
        }
    }
    for (int i = 0; i < 9 && quantity > 0; i++) {
        if (inventory[i].item == NULL || inventory[i].item->id == NONE) {
            int add = (quantity > 99) ? 99 : quantity;
            inventory[i].item = item;
            inventory[i].quantity = add;
            quantity -= add;
        }
    }
    return (quantity == 0) ? 1 : 0;
}


//------------------------------------------------------------------------------
// Removes items at a desired quantity from the inventory.
//------------------------------------------------------------------------------
int removeItemFromInventory(InventorySlot inventory[], ItemType itemType, int quantity) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == itemType) {
            if (inventory[i].quantity > quantity) {
                inventory[i].quantity -= quantity;
            } else {
                inventory[i].item = NULL;
                inventory[i].quantity = 0;
            }
            moveInventoryItemsTogether(inventory);
            return 1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
// Returns the quantity of an item in the inventory if it exists.
//------------------------------------------------------------------------------
int hasItemInInventory(InventorySlot inventory[], ItemType itemType) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == itemType) {
            return inventory[i].quantity;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
// Lets the player know if they are able to plant an item if its a seed.
//------------------------------------------------------------------------------
void drawCanPlantCrop(int itemSelect){
	if (player.inventory[itemSelect - 1].item->subType == CROPSEED){
	    ssd1306_SetCursor(91 - ((int)strlen("plant") * 6 / 2), 40);
	    ssd1306_WriteString("plant", Font_6x8, White);
	} else {
		ssd1306_SetCursor(91 - (7 / 2), 39);
		ssd1306_WriteString("X", Font_7x10, White);
	}
    ssd1306_DrawRectangle(66, 35, 115, 51, White);
    ssd1306_DrawRectangle(67, 36, 114, 50, White);
}

//------------------------------------------------------------------------------
// Lets the player know if they are able to plant an item if its a sapling.
//------------------------------------------------------------------------------
void drawCanPlantSapling(int itemSelect){
	if (player.inventory[itemSelect - 1].item->subType == SAPLING){
	    ssd1306_SetCursor(91 - ((int)strlen("plant") * 6 / 2), 40);
	    ssd1306_WriteString("plant", Font_6x8, White);
	} else {
		ssd1306_SetCursor(91 - (7 / 2), 39);
		ssd1306_WriteString("X", Font_7x10, White);
	}

    ssd1306_DrawRectangle(66, 35, 115, 51, White);
    ssd1306_DrawRectangle(67, 36, 114, 50, White);
}

//------------------------------------------------------------------------------
// Lets the player know if they are able to consume an item if its consumable.
//------------------------------------------------------------------------------
void drawCanConsume(int itemSelect){
	if (player.inventory[itemSelect - 1].item->subType == CONSUMABLE){
	    ssd1306_SetCursor(91 - ((int)strlen("drink") * 6 / 2), 40);
	    ssd1306_WriteString("drink", Font_6x8, White);
	} else {
		ssd1306_SetCursor(91 - (7 / 2), 39);
		ssd1306_WriteString("X", Font_7x10, White);
	}

    ssd1306_DrawRectangle(66, 35, 115, 51, White);
    ssd1306_DrawRectangle(67, 36, 114, 50, White);
}

//------------------------------------------------------------------------------
// Lets the player know if they are able to feed an item to the cat.
//------------------------------------------------------------------------------
void drawCanFeed(int itemSelect){
	if (player.inventory[itemSelect - 1].item->subType == HFRUIT ||
			player.inventory[itemSelect - 1].item->subType == HCROP){
	    ssd1306_SetCursor(91 - ((int)strlen("feed") * 6 / 2), 40);
	    ssd1306_WriteString("feed", Font_6x8, White);
	} else {
		ssd1306_SetCursor(91 - (7 / 2), 39);
		ssd1306_WriteString("X", Font_7x10, White);
	}

    ssd1306_DrawRectangle(66, 35, 115, 51, White);
    ssd1306_DrawRectangle(67, 36, 114, 50, White);
}

//------------------------------------------------------------------------------
// Shows the players inventory, depeding on the int entered it will have different
// functions: Planting Seeds, Planting Saplings, Feeding the cat.
//------------------------------------------------------------------------------
int showInventory(int plantSeed) {
	if (plantSeed == 3) {
		sound(catMeow);
	}
	else {
		sound(inventoryOpen);
	}
	moveInventoryItemsTogether(player.inventory);
    int itemSelect = 1;
    int moved = 1;
    int xMov, yMov;
    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    drawInventoryIcons(xMov, yMov);
    displayStats();
    ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
    drawItemInfo(itemSelect);
    ssd1306_UpdateScreen();
    while (1) {
        if (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0 && itemSelect > 3) {
            moved = 1;
            sound(menuNav);
            itemSelect -= 3;
            while (HAL_GPIO_ReadPin(GPIOB, UP_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0 && itemSelect < 7) {
            moved = 1;
            sound(menuNav);
            itemSelect += 3;
            while (HAL_GPIO_ReadPin(GPIOA, DOWN_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0 && (itemSelect % 3) != 1) {
            moved = 1;
            sound(menuNav);
            itemSelect -= 1;
            while (HAL_GPIO_ReadPin(GPIOB, LEFT_Pin) == 0);
        }
        if (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0 && (itemSelect % 3) != 0) {
            moved = 1;
            sound(menuNav);
            itemSelect += 1;
            while (HAL_GPIO_ReadPin(GPIOB, RIGHT_Pin) == 0);
        }

        if (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 || HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1) {
            while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0 || HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
            break;
        }

        if (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0) {
            while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);

            if (plantSeed == 1) {
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem->subType == CROPSEED) {
                	return selectedItem->id;
                }
            }
            else if (plantSeed == 2) {
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem->subType == SAPLING) {
                	return selectedItem->id;
                }
            }
            else if (plantSeed == 3) {
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem->subType == HFRUIT || selectedItem->subType == HCROP) {
                	cat.love += selectedItem->sellValue;
                	sound(catFeed);
                    ItemType feedItem = selectedItem->id;
                    if (feedItem != NONE) {
                        int slot = -1;
                        for (int i = 0; i < 9; i++) {
                            if (player.inventory[i].item != NULL &&
                                player.inventory[i].item->id == feedItem) {
                                slot = i;
                                break;
                            }
                        }
                        if (slot != -1) {
                            removeItemFromInventory(player.inventory,
                                                    player.inventory[slot].item->id,
                                                    1);
                        }
                    }
                }
            }
            else {
                moved = 1;
            }
            moved = 1;
        }
        if (moved) {
            moved = 0;
            ssd1306_FillRectangle(5, 1, 122, 55, Black);
            ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            drawInventoryIcons(xMov, yMov);
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);
            ssd1306_FillRectangle(57, 2, 121, 53, Black);
            drawItemInfo(itemSelect);
            switch (plantSeed) {
                case 0: displayStats(); break;
                case 1: drawCanPlantCrop(itemSelect);    displayStats();    break;
                case 2: drawCanPlantSapling(itemSelect); displayStats();    break;
                case 3: drawCanFeed(itemSelect);         displayCatStats(); break;
            }
            ssd1306_UpdateScreen();
        }
    }
    return NONE;
}
