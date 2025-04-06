#include "sound.h"


// Harvest sound: a bright ascending arpeggio.
const uint16_t harvestSound[][2] = {
    {A4, 20},
    {C5, 20},
    {E5, 20},
    {0,  60},
    {0,  0}
};

// Menu open: a gentle two-note jump.
const uint16_t menuOpen[][2] = {
    {C5, 50},
    {G5, 50},
    {0,  0}
};

// Menu navigation: a quick, repeating tone.
const uint16_t menuNav[][2] = {
    {E4, 30},
    {E4, 30},
    {0,  0}
};

// Menu select: a two-tone upbeat hit.
const uint16_t menuSelect[][2] = {
    {G5, 40},
    {A5, 40},
    {0,  0}
};

// Menu exit: a descending two-note phrase.
const uint16_t menuExit[][2] = {
    {G5, 50},
    {E5, 50},
    {0,  0}
};

// Saving game: a brief, serious tone.
const uint16_t savingGame[][2] = {
    {F5, 60},
    {0,  0}
};

// Game saved: a confirming tone.
const uint16_t gameSaved[][2] = {
    {C5, 80},
    {E5, 80},
    {G5, 80},
    {A5, 80},
    {G5, 80},
    {E5, 80},
    {C5, 80},
    {0,  80},
    {0,   0}
};

// Sell item: a descending arpeggio.
const uint16_t sellItem[][2] = {
    {C5, 50},
    {G4, 50},
    {E4, 50},
    {0,  50},
    {0,  0}
};

// Buy item: an ascending arpeggio.
const uint16_t buyItem[][2] = {
    {E4, 50},
    {G4, 50},
    {C5, 50},
    {0,  50},
    {0,  0}
};

// cantBuy: a buzzy repeated tone.
const uint16_t cantBuy[][2] = {
    {A4, 30},
    {A4, 30},
    {A4, 30},
    {0,  30},
    {0,  0}
};

// cantSell: similar error tone at a lower pitch.
const uint16_t cantSell[][2] = {
    {F4, 30},
    {F4, 30},
    {F4, 30},
    {0,  0}
};

// areYouSure: a questioning two-tone sequence.
const uint16_t areYouSure[][2] = {
    {D5, 70},
    {C5, 70},
    {0,  0}
};

// deleteSave: a somber two-tone.
const uint16_t deleteSave[][2] = {
    {F4, 80},
    {E4, 80},
    {0,  0}
};

// buttonPress: a quick, crisp two-tone.
const uint16_t buttonPress[][2] = {
    {B4, 40},
    {C5, 40},
    {0,  0}
};

// playGame: a rhythmic three-note pattern.
const uint16_t playGame[][2] = {
    {G4, 50},
    {C5, 50},
    {E5, 50},
    {0,  0}
};

// levelUp: an ascending scale for leveling up.
const uint16_t levelUp[][2] = {
    {C5, 50},
    {E5, 70},
    {G5, 70},
    {A5, 70},
    {B5, 70},
    {1047, 100},
    {0,  0}
};

// harvest (alternate): a bright, upbeat sequence.
const uint16_t harvest[][2] = {
    {G4, 50},
    {B4, 50},
    {D5, 50},
    {0,  0}
};

// destroy: a descending three-note phrase.
const uint16_t destroy[][2] = {
    {E5, 60},
    {C5, 60},
    {A4, 60},
    {0,  0}
};

// inventoryOpen: a short, welcoming two-tone.
const uint16_t inventoryOpen[][2] = {
    {C5, 40},
    {E5, 40},
    {0,  0}
};

// inventoryFull: a repeated low note to indicate full inventory.
const uint16_t inventoryFull[][2] = {
    {D4, 40},
    {D4, 40},
    {D4, 40},
    {0,  0}
};

// mapOpen: a repeated low note to indicate full inventory.
const uint16_t mapOpen[][2] = {
    {E4,  30},
    {G4s, 40},
    {B4,  50},
    {0,  0}
};

// mapOpen: a repeated low note to indicate full inventory.
const uint16_t catMeow[][2] = {
    {G5, 70},
    {E5, 100},
    {0,  0}
};

// catFeed: a gentle one-note.
const uint16_t catFeed[][2] = {
    {G5, 50},
    {0,  0}
};

// catSit: a descent.
const uint16_t catSit[][2] = {
    {F5, 70},
    {E5, 70},
    {D5, 70},
    {C5, 100},
    {0,  0}
};

// catSit: a descent.
const uint16_t catStand[][2] = {
    {C5, 70},
    {D5, 70},
    {E5, 70},
    {F5, 100},
    {0,  0}
};

//------------------------------------------------------------------------------
// Plays a selected sound from the sound library.
//------------------------------------------------------------------------------
void sound(const uint16_t soundArray[][2]) {
    int i = 0;
    while (soundArray[i][0] != 0 || soundArray[i][1] != 0) {
        if (soundArray[i][0] != 0) {
            // Play the note (frequency, duration)
            buzzer(soundArray[i][0], soundArray[i][1]);
        } else {
            // Rest for the specified duration
            HAL_Delay(soundArray[i][1]);
        }
        i++;
    }
}
