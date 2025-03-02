#ifndef GAME_H_
#define GAME_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "sprites.h"
#include "crop.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ONLY DEFINE 1 //
#define CPP		//ssd1306 lib
//#define XBM  //u8g2 lib

extern int statbarShow;
extern int batteryLife; //number 1 to 10;
extern int leaveWorld;
extern int refresh;
extern int soundOn;
extern int selectedInventorySlot; // Track the selected slot in the inventory

typedef enum {
	NONE,
    HOUSEKEY,
	TILLSOIL,
    WHEAT,
    CORN,
    POTATO,
    CARROT,
    PUMPKIN,
    SUGAR,
    WHEATSEED,
    CORNSEED,
    POTATOSEED,
    CARROTSEED,
    PUMPKINSEED,
    SUGARSEED
} ItemType;

typedef struct {
    ItemType id;
    int sellValue;
    int buyValue;
    int growTime;      // in game ticks
    int xp;
    int levelUnlock;
    int quantity;
    const unsigned char *sprite;     // Normal (grown) sprite
    const unsigned char *seedSprite; // Seed sprite (if applicable; NULL if not)
} Item;

extern Item wheat, corn, potato, carrot, pumpkin, sugar;
extern Item wheatSeed, cornSeed, potatoSeed, carrotSeed, pumpkinSeed, sugarSeed;
extern Item tillSoil, houseKey;

extern Item shopItems[];

typedef enum {
    TITLE,
    CROP,
    SHOP,
    ORCHARD,
    FISHING
} World;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct {
    int x;
    int y;
} Coordinates;

typedef struct {
    Item* item;
    int quantity;
} InventorySlot;

// Attributes of a player
typedef struct {
    int money;
    World inWorld;
    int xp;
    int level;
    Coordinates coordinates;
    Direction direction;
    InventorySlot inventory[9];
    int soilSpots;
} Player;

// Attributes of the game
typedef struct {
    int startTimes;
} Game;

// Attributes of a croptile
typedef struct {
    Item crop;      // The crop planted (if any)
    int grown;      // Growth progress (0 if not planted)
    bool isTilled;  // Whether the soil is tilled
} CropTile;

// Parameters for initializing crop world
typedef struct {
    int numGameTicks; // Assuming this is what you meant
} PlayerCropInitParams;

// Enum for buy menu navigation directions
typedef enum {
    Left,
    Right,
    Up,
    Down
} BuyMenuDirection;

extern Player player;

extern CropTile cropTiles[10];

extern Direction cropDirection;


void displayStats();
int gameMenu();
int addItemToInventory(InventorySlot inventory[], Item *item, int quantity);
int removeItemFromInventory(InventorySlot inventory[], ItemType itemType, int quantity);
int hasItemInInventory(InventorySlot inventory[], ItemType itemType);
Item* getItemByType(ItemType itemType);
void gameOptions(void);

#endif /* INC_GAME_H_ */
