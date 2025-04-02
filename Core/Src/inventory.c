#include "inventory.h"

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

void drawItemInfo(int itemSelect){
    // Draw the title and quantity of the selected item
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


bool isInventoryEmpty(InventorySlot inventory[]) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id != NONE) {
            return false;
        }
    }
    return true;
}

void moveInventoryItemsTogether(InventorySlot inventory[]) {
    int nextFree = 0;
    for (int i = 0; i < 9; i++) {
        // Check if the current slot is not empty
        if (inventory[i].item != NULL && inventory[i].item->id != NONE) {
            if (i != nextFree) {
                // Move the item and its quantity to the next available free slot
                inventory[nextFree] = inventory[i];
                // Mark the original slot as empty
                inventory[i].item = NULL;
                inventory[i].quantity = 0;
            }
            nextFree++;
        }
    }
}

// Add an item to the inventory
int addItemToInventory(InventorySlot inventory[], Item *item, int quantity) {
    // First, try to add to existing stacks that are not full.
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == item->id && inventory[i].quantity < 99) {
            int space = 99 - inventory[i].quantity;
            int add = (quantity > space) ? space : quantity;
            inventory[i].quantity += add;
            quantity -= add;
            if (quantity == 0) return 1; // All quantity added.
        }
    }

    // Then, add remaining quantity in new empty slots.
    for (int i = 0; i < 9 && quantity > 0; i++) {
        if (inventory[i].item == NULL || inventory[i].item->id == NONE) {
            int add = (quantity > 99) ? 99 : quantity;
            inventory[i].item = item;
            inventory[i].quantity = add;
            quantity -= add;
        }
    }

    // Return success if all quantity was added.
    return (quantity == 0) ? 1 : 0;
}


// Remove an item from the inventory
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
            return 1; // Success
        }
    }
    return 0; // Item not found
}

// Check if an item exists in the inventory
int hasItemInInventory(InventorySlot inventory[], ItemType itemType) {
    for (int i = 0; i < 9; i++) {
        if (inventory[i].item != NULL && inventory[i].item->id == itemType) {
            return inventory[i].quantity;
        }
    }
    return 0; // Item not found
}

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

int showInventory(int plantSeed) {

	moveInventoryItemsTogether(player.inventory);

    int itemSelect = 1; // Start with the first item selected
    int moved = 1;      // Indicates that the selection box needs to be redrawn
    int xMov, yMov;
    int prevXMov = 0, prevYMov = 0; // Track previous selection box position

    ssd1306_FillRectangle(5, 1, 122, 55, Black);
    ssd1306_DrawBitmap(4, 0, BoardSprite, 120, 60, White);

    xMov = ((itemSelect - 1) % 3) * 17;
    yMov = ((itemSelect - 1) / 3) * 17;
    prevXMov = xMov;
    prevYMov = yMov;

    drawInventoryIcons(xMov, yMov);
    displayStats();

    // Draw the initial selection box
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
                // Check if the selected item is a valid seed
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem->subType == CROPSEED) {
                    // Remove one unit of the seed from the inventory.
                	return selectedItem->id;
                }
            }
            else if (plantSeed == 2) {
                // Check if the selected item is a valid sapling
                Item* selectedItem = player.inventory[itemSelect - 1].item;
                if (selectedItem->subType == SAPLING) {
                    // Remove one unit of the sapling from the inventory.
                	return selectedItem->id;
                }
            }
            else {
                // Handle normal inventory selection if needed.
                moved = 1; // Ensure redraw after selection.
            }
            moved = 1; // Ensure redraw after selection.
        }

        if (moved) {
            moved = 0;
            // Erase the previous selection box.
            ssd1306_DrawRectangle(8 + prevXMov, 4 + prevYMov, 21 + prevXMov, 17 + prevYMov, Black);

            // Update selection box position.
            xMov = ((itemSelect - 1) % 3) * 17;
            yMov = ((itemSelect - 1) / 3) * 17;
            prevXMov = xMov;
            prevYMov = yMov;

            displayStats();

            // Draw the new selection box.
            ssd1306_DrawRectangle(8 + xMov, 4 + yMov, 21 + xMov, 17 + yMov, White);

            // Clear previous text area.
            ssd1306_FillRectangle(57, 2, 121, 53, Black);

            drawItemInfo(itemSelect);

            if (plantSeed == 1) {
            	drawCanPlantCrop(itemSelect);
            }
            else if (plantSeed == 2){
            	drawCanPlantSapling(itemSelect);
            }
            else {
            		//drawCanConsume(itemSelect);
            }

            ssd1306_UpdateScreen();
        }
    }
    // If no valid seed is selected, return NONE.
    return NONE;
}
