#include "game.h"
#include "crop.h"
#include "sprites.h"
#include "crop_house.h"
#include <stdbool.h>

#define NEAR_THRESHOLD 1

void cropHouseDisplay(){
    // Draw the house background bitmap
    ssd1306_DrawBitmap(0, 0, TheHouse, 128, 64, White);
    if (game.mileStone < MAP_ACQUIRED){
    	ssd1306_DrawBitmap(20, 34, MapOnTable, 16, 5, White);
    }

    ssd1306_DrawBitmap(9, 50, BasementDoor, 25, 13, White);

    // Prepare the player's coordinates string
    char coordString[20];
    sprintf(coordString, "X:%d Y:%d", player.coordinates.x, player.coordinates.y);

    // Set the cursor position to an area that doesn't interfere with the main graphic.
    // Here, we choose a y value of 56 (near the bottom) so that the text is visible.
    ssd1306_SetCursor(0, 0);

    // Write the coordinate string on the display using a chosen font (e.g., Font_6x8)
    //ssd1306_WriteString(coordString, Font_6x8, White);
}

bool cropHouseObstacle(int x, int y) {
    // Obstacle 1: Arcade machine.
    if (x >= 62 && x <= 79 && y >= 0 && y <= 18)
        return true;

    // Obstacle 2: Bookshelf
    if (x >= 18 && x <= 57 && y >= 0 && y <= 18)
        return true;

    // Obstacle 3: Table
    if (x >= 12 && x <= 37 && y >= 22 && y <= 36)
        return true;

//    // Obstacle 4: Stairwell
//    if (((x >= 0 && x <= 30) && (y >= 42 && y <= 46)) || ((y >= 51 && y <= 53) && (x >= 0 && x <= 26)))
//        return true;

    return false;
}

bool nearTable() {
    int left = 12, right = 38, top = 22, bottom = 36;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearBookshelf() {
    int left = 18, right = 57, top = 0, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearArcadeMachine() {
    int left = 61, right = 80, top = 0, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearRasins() {
    int left = 102, right = 107, top = 26, bottom = 32;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearPee() {
    int left = 97, right = 107, top = 44, bottom = 52;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}

bool nearDoor() {
    int left = 89, right = 97, top = 17, bottom = 18;
    int px = player.coordinates.x;
    int py = player.coordinates.y;

    if (px >= (left - NEAR_THRESHOLD) && px <= (right + NEAR_THRESHOLD) &&
        py >= (top - NEAR_THRESHOLD) && py <= (bottom + NEAR_THRESHOLD))
        return true;

    return false;
}


void cropHouseIntro(){
	textSpeaking("its a dump in here", 150, 8, 1);
	textSpeaking("(it smells like     stale bread)", 150, 8, 1);
	textSpeaking("i wonder who used to live here...", 150, 8, 1);
}

void cropHousePlayerMovement(){
    uint8_t step = 1;  // Fixed movement step

    // UP button
    if (UP_Button_Flag) {
        UP_Button_Flag = 0;
        player.direction = UP;
        int nextY = player.coordinates.y - step;
        if (nextY > TOP_SCREEN_EDGE + 16 && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // DOWN button
    if (DOWN_Button_Flag) {
        DOWN_Button_Flag = 0;
        player.direction = DOWN;
        int nextY = player.coordinates.y + step;
        if (nextY < BOTTOM_WORLD_EDGE - 1 && !cropHouseObstacle(player.coordinates.x, nextY))
            player.coordinates.y = nextY;
    }

    // LEFT button
    if (LEFT_Button_Flag) {
        LEFT_Button_Flag = 0;
        player.direction = LEFT;
        int nextX = player.coordinates.x - step;
        if (nextX > LEFT_WORLD_EDGE + 12 && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }

    // RIGHT button
    if (RIGHT_Button_Flag) {
        RIGHT_Button_Flag = 0;
        player.direction = RIGHT;
        int nextX = player.coordinates.x + step;
        if (nextX < RIGHT_WORLD_EDGE - 12 && !cropHouseObstacle(nextX, player.coordinates.y))
            player.coordinates.x = nextX;
    }
}

void mapInteract(){
	if (game.mileStone < MAP_ACQUIRED){
		game.mileStone = MAP_ACQUIRED;
		textSpeaking("interesting... a map", 150, 8, 1);
		textSpeaking("woah, what???       there's something   else???", 150, 8, 1);
		textSpeaking("(press start to use the map)", 150, 8, 1);
		textSpeaking("(the map allows you to fast travel      around the world)", 150, 8, 1);
		textSpeaking("(maybe go visit the orchard? or do whatever I don't care)", 150, 8, 1);
		textSpeaking("(Till More Soil to  plant saplings at   the orchard btw)", 150, 8, 1);
		ssd1306_Fill(Black);        // Clear the screen
		ssd1306_DrawBitmap(0, 0, ControlsTitle2, 128, 64, White);
		ssd1306_UpdateScreen();
		HAL_Delay(250);
		while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 1);
	} else {
		textSpeaking("what a fine walnut  table!", 150, 8, 1);
		textSpeaking("(you already got the map quit looking at the table...)", 150, 8, 1);
	}
}

void booksOfLore(){
	textSpeaking("most of these are   porn, weird.", 150, 8, 1);
	textSpeaking("there is a diary    here though...", 150, 8, 1);
	textSpeaking("jeez this thing is  wordy, get ready    for a read", 150, 8, 1);
	if (!textPrompt("Read the diary?")){
		return;
	}
    textSpeakingFullScreen("1/1/2293 - Entry 1", 100, 10, 1);
    textSpeakingFullScreen("Hello, my name is Kiki, I am writing this diary to keep track of things!" , 100, 10, 1);
    textSpeakingFullScreen("So far the farm seems to be doing very well, everyone is getting fed." , 100, 10, 1);
    textSpeakingFullScreen("We have had some minor set backs but as of now everything is going good!" , 100, 10, 1);
    textSpeakingFullScreen("Anyways, thats all for now, ciao!" , 100, 10, 1);

    textSpeakingFullScreen("2/4/2293 - Entry 2", 100, 10, 1);
    textSpeakingFullScreen("Hello diary, its been about a month now, we recently started planting seeds at the orchard." , 100, 10, 1);
    textSpeakingFullScreen("The soil here is super fertile! with how well the farm has been doing I hope these seeds take!" , 100, 10, 1);
    textSpeakingFullScreen("Tom the shopkeeper is feeling a little ill, I hope he feels better soon!" , 100, 10, 1);

    textSpeakingFullScreen("3/21/2293 - Entry 3", 100, 10, 1);
    textSpeakingFullScreen("The orchard is doing amazing!!! The trees are bearing great fruit, it sure is a morale boost" , 100, 10, 1);
    textSpeakingFullScreen("On a less positive note... Tom is really really sick, he has not been able to tend to the shop" , 100, 10, 1);
    textSpeakingFullScreen("... his wife Lucy has been taking care of his shop in the meantime, I really hope he gets better" , 100, 10, 1);
    textSpeakingFullScreen("Oh... Lucy just called, I guess thats all for today" , 100, 10, 1);

    textSpeakingFullScreen("6/3/2293 - Entry 4", 100, 10, 1);
    textSpeakingFullScreen("I forgot about this thing already, whoops" , 100, 10, 1);
    textSpeakingFullScreen("Tom just died. People have been losing hope around here, I never accounted for this to happen" , 100, 10, 1);
    textSpeakingFullScreen("... so soon. Lucy has taken over both shops and I can tell its really getting to her." , 100, 10, 1);
    textSpeakingFullScreen("It may be time to start trying to finish my helper bots, she could really use the help." , 100, 10, 1);
    textSpeakingFullScreen("God let there be hope..." , 100, 10, 1);

    textSpeakingFullScreen("9/13/2293 - Entry 5", 100, 10, 1);
    textSpeakingFullScreen("I finished my first model of my helper bot. I modeled artificial skin to it" , 100, 10, 1);
    textSpeakingFullScreen("I was able to use my old scans of people to create a perfect replica of him..." , 100, 10, 1);
    textSpeakingFullScreen("This feels so wrong, but Lucy seems to be a lot more at ease these days." , 100, 10, 1);
    textSpeakingFullScreen("Am I pushing the limits?" , 100, 10, 1);

    textSpeakingFullScreen("11/2/2293 - Entry 6", 100, 10, 1);
    textSpeakingFullScreen("Tom's helper bot is remarkably still working great." , 100, 10, 1);
    textSpeakingFullScreen("Its super eerie seeing Lucy so in love with it, she kisses him every day." , 100, 10, 1);
    textSpeakingFullScreen("Lucy seems to have lost it a little, clinging on to shards of Tom' memory." , 100, 10, 1);
    textSpeakingFullScreen("She has been super scattered, she has been selling the wrong things to people." , 100, 10, 1);
    textSpeakingFullScreen("I think I even saw her eating raw seeds, super weird stuff." , 100, 10, 1);

    textSpeakingFullScreen("5/6/2294 - Entry 7", 100, 10, 1);
    textSpeakingFullScreen("Lucy has lost her mind." , 100, 10, 1);
    textSpeakingFullScreen("She truly is not herself, all the denial has finally washed away." , 100, 10, 1);
    textSpeakingFullScreen("She has not left her house in 2 months." , 100, 10, 1);
    textSpeakingFullScreen("Every time I check up on her she just stares at the wall... all day long." , 100, 10, 1);
    textSpeakingFullScreen("I programmed Tom's helper bot to tend to both stores..." , 100, 10, 1);
    textSpeakingFullScreen("although I think his system is overloaded, he over heats constantly." , 100, 10, 1);
    textSpeakingFullScreen("Maybe its time for a Lucy helper bot, I'm sure she wouldn't mind." , 100, 10, 1);
    textSpeakingFullScreen("Not like she's really around much anyway..." , 100, 10, 1);

    textSpeakingFullScreen("7/4/2294 - Entry 8", 100, 10, 1);
    textSpeakingFullScreen("The Lucy helper bot has beeen running the store great!" , 100, 10, 1);
    textSpeakingFullScreen("After hours I programmed her bot to help feed and bathe her, she still is not right..." , 100, 10, 1);
    textSpeakingFullScreen("It has been 4 months now. Lucy is still cooped up in there." , 100, 10, 1);
    textSpeakingFullScreen("I have been talking a lot to the helper bots, they're kind of my only company these days" , 100, 10, 1);
    textSpeakingFullScreen("It just isn't the same anymore... Things started so well." , 100, 10, 1);
    textSpeakingFullScreen("Am I an idiot for thinking we could survive after the war? Should we have just killed ourselves?" , 100, 10, 1);

    textSpeakingFullScreen("3/21/2295 - Entry 9", 100, 10, 1);
    textSpeakingFullScreen("Lucy died this morning." , 100, 10, 1);
    textSpeakingFullScreen("Can't say much has changed, life has still been going on" , 100, 10, 1);
    textSpeakingFullScreen("I created 2 more robots to help me tend to my needs, one is equipped for medical services" , 100, 10, 1);
    textSpeakingFullScreen("... you know, in case I get sick too." , 100, 10, 1);
    textSpeakingFullScreen("The other one is my partner. I need someone to love me." , 100, 10, 1);
    textSpeakingFullScreen("I really feel myself slipping... I don't know if I can go on" , 100, 10, 1);

    textSpeakingFullScreen("5/1/2295 - Entry 10", 100, 10, 1);
    textSpeakingFullScreen("I cant take this anymore." , 100, 10, 1);
    textSpeakingFullScreen("Not another day can go by, I need a change now or else." , 100, 10, 1);
    textSpeakingFullScreen("I have started on a memory wiping device, I'm going to test it this afternoon." , 100, 10, 1);
    textSpeakingFullScreen("Hopefully this will help me forget." , 100, 10, 1);
    textSpeakingFullScreen("(There are no more entries after this.)" , 100, 10, 1);
}

void cropHousePlayerAction(){
    // Button A:
    if (A_Button_Flag) {
    	A_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
        if (nearArcadeMachine()) textSpeaking("a broken arcade     machine, wonder if  it can be fixed...", 150, 8, 1);
        if (nearBookshelf()) booksOfLore();
        if (nearTable()) mapInteract();
        if (nearRasins()) textSpeaking("ew ew ew,           raisins on the floor", 150, 8, 1);
        if (nearPee()) textSpeaking("piss on the floor,  real classy", 150, 8, 1);
        if (nearDoor()) textSpeaking("another locked door, great...", 150, 8, 1);
        while (HAL_GPIO_ReadPin(GPIOB, A_Pin) == 0);
    }

    // Button B:
    if (B_Button_Flag) {
    	B_Button_Flag = 0;
    	refreshBackground = 1;
        while (HAL_GPIO_ReadPin(GPIOB, B_Pin) == 0);
        buzzer(300, 25);
        showInventory(0);
    }

    // SELECT button: Enter menu mode.
    if (SELECT_Button_Flag) {
    	SELECT_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, SELECT_Pin) == 0);
        statbarShow = 0;
        refreshBackground = 1;
        int p = gameMenu();  // Assume gameMenu() returns a flag.
        if (p)
            leaveWorld = 1;
    }

    // START button:
    if (START_Button_Flag && game.mileStone >= MAP_ACQUIRED) {
    	START_Button_Flag = 0;
        while (HAL_GPIO_ReadPin(GPIOA, START_Pin) == 1);
        theMap();
    }
}

void handleCropHouse() {
    // Set starting position and direction

    ssd1306_Fill(Black);
    cropHouseDisplay();
    catDisplay();
	playerDisplay();
	ssd1306_UpdateScreen();

	if (game.cropHouseIntro){
		cropHouseIntro();
		game.cropHouseIntro = 0;
	}

    leaveWorld = 0;
    uint32_t lastFrameTime = HAL_GetTick();
    const uint32_t FRAME_DELAY = FrameRate;  // ~30 FPS

    while (!leaveWorld) {
        uint32_t now = HAL_GetTick();
        if (now - lastFrameTime >= FRAME_DELAY) {
            // Process input and update state only once per frame
        	ssd1306_Fill(Black);

        	cropHouseDisplay();

        	updateButtonFlags();
        	cropHousePlayerMovement();

            catDisplay();
        	playerDisplay();

        	cropHousePlayerAction();

        	ssd1306_UpdateScreen();
            lastFrameTime = now;
        }

        gameLogic();

        HAL_Delay(1);

        if (worldBreak) {
        	worldBreak = 0;
        	break;
        }

        // Exit condition: if player exits the house
        if (player.coordinates.y >= 52 &&
           (player.coordinates.x + 5 > 60) &&
           (player.coordinates.x < 60 + 6)) {
            player.inWorld = CROP;
            player.coordinates.x = 12;
            player.coordinates.y = 17;
            player.direction = DOWN;
            break;
        }

    }
}
