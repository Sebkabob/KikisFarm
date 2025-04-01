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

#define TOP_SCREEN_EDGE 0
#define BOTTOM_WORLD_EDGE 54
#define LEFT_WORLD_EDGE -1
#define RIGHT_WORLD_EDGE 121

#define NEW_GAME 				0
#define FIRST_CROP_PLANTED 		5
#define FIRST_CROP_HARVEST 		10
#define FIRST_SHOP_VISIT 		15
#define FIRST_SELL_VISIT 		20
#define HOUSE_KEY_BOUGHT 		25
#define HOUSE_ENTERED 			30
#define MAP_ACQUIRED 			35
#define ORCHARD_ENTERED 		40
#define FIRST_TREE_PLANTED 		45
#define FIRST_TREE_HARVESTED	50
#define BOAT_ACQUIRED			55

// ONLY DEFINE 1 //
#define CPP		//ssd1306 lib
//#define XBM  //u8g2 lib

extern int statbarShow;
extern int batteryLife; //number 1 to 10;
extern int leaveWorld;
extern int refresh;
extern int soundOn;
extern int batteryLow;
extern int batteryPercent;
extern int selectedInventorySlot; // Track the selected slot in the inventory

extern int FrameRate;
extern int GrowSpeed;


extern int A_Button_Flag;
extern int B_Button_Flag;
extern int START_Button_Flag;
extern int SELECT_Button_Flag;
extern int UP_Button_Flag;
extern int DOWN_Button_Flag;
extern int LEFT_Button_Flag;
extern int RIGHT_Button_Flag;

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
	SAFFRON,
	MINT,
	TOMATO,
    WHEATSEED,
    CORNSEED,
    POTATOSEED,
    CARROTSEED,
    PUMPKINSEED,
    SUGARSEED,
	SAFFRONSEED,
	MINTSEED,
	TOMATOSEED,
	APPLE,
	ORANGE,
	BANANA,
	CHERRY,
	MONEY,
	APPLESAPLING,
	ORANGESAPLING,
	BANANASAPLING,
	CHERRYSAPLING,
	MONEYSAPLING,
	BOAT
} ItemType;

typedef enum {
	HCROP,
	CROPSEED,
	SERVICE,
	CONSUMABLE,
	ITEM,
	HFRUIT,
	SAPLING
} SubItemType;

typedef enum {
    TITLE,
    CROP,
    SHOP,
	CROPHOUSE,
	HOUSEROOM,
	BASEMENT,
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
    ItemType id;
    int sellValue;
    int buyValue;
    int growTime;      // in game ticks
    int xp;
    int levelUnlock;
    SubItemType subType;
    const unsigned char *cropSprite;     // Normal (grown) sprite
    const unsigned char *itemSprite; // Seed sprite (if applicable; NULL if not)
    const unsigned char *titleSprite;
} Item;

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
	int ticks;
	int day;
    int houseUnlocked;		//1 if the player buys the key
    int firstIntro;			//0 once the intro has been seen
    int cropHouseIntro;		//0 once the intro has been seen
    int mileStone;
} Game;

// Attributes of a croptile
typedef struct {
    Item crop;      // The crop planted (if any)
    int grown;      // Growth progress (0 if not planted)
    bool isTilled;  // Whether the soil is tilled
} CropTile;

// Attributes of a croptile
typedef struct {
    Item tree;      // The tree planted (if any)
    int grown;      // Growth progress (0 if not planted)
    bool isTilled;  // Whether the soil is tilled
} TreeTile;

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

extern int worldBreak;

extern int refreshBackground;

extern Item shopItems[];

extern Item wheat, corn, potato, carrot, pumpkin, sugar, saffron, mint, tomato;
extern Item wheatSeed, cornSeed, potatoSeed, carrotSeed, pumpkinSeed, sugarSeed, saffronSeed, mintSeed, tomatoSeed;
extern Item tillSoil, houseKey;

extern Player player;

extern Game game;

extern CropTile cropTiles[10];

extern Direction cropDirection;


Item* getItemPointerFromID(ItemType id);
Item getGrownCrop(ItemType seedId);
int getTillSoilCost(void);
void displayLevelUp(void);
int gameLevelUp(void);
void gameStartup(void);
void initGame(void);
void initShopItems(void);
void updateButtonFlags(void);
void cropGrowth(void);
void gameLogic(void);
void playerDisplay(void);
void playerErase(void);
void textSpeakingFullScreen(const char *text, int voice, int speed, int button);
void textSpeaking(const char *text, int voice, int speed, int button);
void cutToDark(int speed);
void displayStats(void);
int gameMenu(void);
void gameOptions(void);


#endif /* INC_GAME_H_ */
