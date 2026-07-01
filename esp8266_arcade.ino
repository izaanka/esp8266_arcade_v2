#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ESP8266 Pins
const int btnLeft = 14;   // D5
const int btnSelect = 12; // D6
const int btnRight = 13;  // D7

// --- Game Engine Base & Registry ---
class ArcadeGame {
public:
    virtual const char* getName() = 0;
    virtual void init() = 0;
    virtual void update() = 0;
    virtual int getScore() { return 0; }
};

#define MAX_GAMES 100
ArcadeGame* gamesRegistry[MAX_GAMES];
int registeredGamesCount = 0;

class GameRegistryEntry {
public:
    GameRegistryEntry(ArcadeGame* game) {
        if (registeredGamesCount < MAX_GAMES) {
            gamesRegistry[registeredGamesCount++] = game;
        }
    }
};

#define REGISTER_GAME(GameClass) \
    GameClass instance_##GameClass; \
    GameRegistryEntry entry_##GameClass(&instance_##GameClass);


// Forward Declaration for Games to return to menu
void returnToMenu();


// --- Game Includes ---
// To add a new game, simply drop its .h file in the folder and #include it here.
// It will automatically register itself to the main menu!
#include "LightsOut.h"
#include "Breakout.h"
#include "BlockJump.h"
#include "SimonSays.h"
#include "Minesweeper.h"
#include "SpaceShooter.h"
#include "TicTacToe.h"
#include "Tetris.h"
#include "FlappyBird.h"
#include "Pacman.h"
#include "ReactionTime.h"
#include "Stacker.h"
#include "GeometryDash.h"
#include "Pinball.h"
#include "Snake.h"
#include "DoodleJump.h"
#include "Asteroids.h"
#include "Frogger.h"
#include "Pong.h"
#include "LunarLander.h"
#include "Racing.h"
#include "WhackAMole.h"
#include "Catch.h"
#include "MissileCommand.h"
#include "Memory.h"
#include "Hangman.h"
#include "Artillery.h"
#include "Rhythm.h"
#include "FallDown.h"
#include "Tron.h"
#include "Submarine.h"
#include "MathQuiz.h"
#include "Mastermind.h"
#include "Game2048.h"
#include "Sokoban.h"
#include "Connect4.h"
#include "Blackjack.h"
#include "Slots.h"
#include "Bowling.h"
#include "TRex.h"
#include "Helicopter.h"
#include "Invaders.h"
#include "DuckHunt.h"
#include "Parachute.h"
#include "RiverRaid.h"
#include "Rover.h"
#include "Skiing.h"
#include "BopIt.h"
#include "Timber.h"
#include "Maze.h"
#include "Darts.h"
#include "NinjaClimb.h"
#include "Fishing.h"
#include "LaserReflect.h"
#include "LockPicker.h"
#include "BalancingAct.h"
#include "Golf.h"
#include "Minecart.h"
#include "JumpRope.h"
#include "Hurdles.h"
#include "BalloonPop.h"
#include "Sumo.h"
#include "IceSkipping.h"
#include "FruitSlice.h"
#include "GoldMiner.h"
#include "CraneGame.h"
#include "DefuseBomb.h"
#include "WireLoop.h"
#include "JavelinThrow.h"
#include "SkateTrick.h"


// --- Main Program ---
int masterState = 0; // 0: Master Menu, 1: Playing Game, 2: Settings, 3: Highscores
int masterMenuOption = 0;
ArcadeGame* currentGame = nullptr;

bool enableHighScores = false;
int highscores[MAX_GAMES] = {0};

void returnToMenu() {
    if (enableHighScores && currentGame != nullptr) {
        int s = currentGame->getScore();
        if (s > highscores[masterMenuOption]) {
            highscores[masterMenuOption] = s;
            EEPROM.put(1 + masterMenuOption * sizeof(int), s);
            EEPROM.commit();
        }
    }
    masterState = 0;
    currentGame = nullptr;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  
  Wire.begin(5, 4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextWrap(false); // Fix text overflowing to next line
  randomSeed(RANDOM_REG32); 

  EEPROM.begin(512);
  uint8_t initByte = EEPROM.read(0);
  if (initByte == 1) enableHighScores = true;
  else enableHighScores = false;
  
  for (int i = 0; i < MAX_GAMES; i++) {
      EEPROM.get(1 + i * sizeof(int), highscores[i]);
      if (highscores[i] < 0 || highscores[i] > 999999) highscores[i] = 0; // Sanity check
  }
}

void loop() {
  // ==========================================
  // HOME SCREEN GLOBAL INTERRUPT (D5 Long Press)
  // ==========================================
  static unsigned long d5Timer = 0;
  if (digitalRead(btnLeft) == LOW) {
    if (d5Timer == 0) d5Timer = millis();
    else if (millis() - d5Timer > 1000) { 
      returnToMenu();
      return; 
    }
  } else {
    d5Timer = 0;
  }

  // ==========================================
  // MASTER MENU STATE
  // ==========================================
  if (masterState == 0) {
    int hsIdx = registeredGamesCount;
    int setIdx = registeredGamesCount + 1;

    if (digitalRead(btnLeft) == LOW) { 
        masterMenuOption = (masterMenuOption > 0) ? masterMenuOption - 1 : setIdx; 
        delay(150); 
    }
    if (digitalRead(btnRight) == LOW) { 
        masterMenuOption = (masterMenuOption < setIdx) ? masterMenuOption + 1 : 0; 
        delay(150); 
    }
    if (digitalRead(btnSelect) == LOW) {
        delay(200);
        if (masterMenuOption == setIdx) {
            masterState = 2; // Settings
        } else if (masterMenuOption == hsIdx) {
            masterState = 3; // Highscores
        } else if (registeredGamesCount > 0) {
            currentGame = gamesRegistry[masterMenuOption];
            currentGame->init();
            masterState = 1;
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 0); display.print("- ARCADE MENU -");
    
    // Dynamic Scrolling Menu logic
    int startItem = masterMenuOption >= 4 ? masterMenuOption - 3 : 0;
    
    for (int i = 0; i < 4; i++) {
      int idx = startItem + i;
      if (idx > setIdx) break;
      display.setCursor(10, 15 + (i * 12));
      if (masterMenuOption == idx) display.print("> ");
      
      if (idx < hsIdx) {
        display.print(idx + 1); display.print(". "); display.print(gamesRegistry[idx]->getName());
      } else if (idx == hsIdx) {
        display.print(idx + 1); display.print(". Highscores");
      } else {
        display.print(idx + 1); display.print(". Settings");
      }
    }
    
    display.display();
  }
  
  // ==========================================
  // PLAYING GAME STATE
  // ==========================================
  else if (masterState == 1) {
      if (currentGame != nullptr) {
          currentGame->update();
      }
  }

  // ==========================================
  // SETTINGS STATE
  // ==========================================
  else if (masterState == 2) {
      if (digitalRead(btnLeft) == LOW || digitalRead(btnRight) == LOW) {
          enableHighScores = !enableHighScores;
          EEPROM.write(0, enableHighScores ? 1 : 0);
          EEPROM.commit();
          delay(200);
      }
      if (digitalRead(btnSelect) == LOW) { delay(200); returnToMenu(); }

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(30, 0); display.print("- SETTINGS -");
      
      display.setCursor(0, 15);
      display.print("< Highscores: ");
      display.print(enableHighScores ? "ON" : "OFF");
      display.print(" >");
      
      display.setCursor(0, 30);
      display.print("WARN: Saving scores");
      display.setCursor(0, 40);
      display.print("reduces flash memory");
      display.setCursor(0, 50);
      display.print("lifespan over time.");
      
      display.display();
  }
  // ==========================================
  // HIGHSCORES STATE
  // ==========================================
  else if (masterState == 3) {
      static int scroll = 0;
      if (digitalRead(btnLeft) == LOW) { scroll = (scroll > 0) ? scroll - 1 : registeredGamesCount - 1; delay(150); }
      if (digitalRead(btnRight) == LOW) { scroll = (scroll < registeredGamesCount - 1) ? scroll + 1 : 0; delay(150); }
      if (digitalRead(btnSelect) == LOW) { delay(200); returnToMenu(); }
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(20, 0); display.print("- HIGHSCORES -");
      
      if (!enableHighScores) {
          display.setCursor(0, 20); display.print("Highscores are OFF.");
          display.setCursor(0, 30); display.print("Enable in Settings");
      } else {
          int start = scroll;
          for(int i=0; i<4; i++) {
              int idx = start + i;
              if (idx >= registeredGamesCount) break;
              display.setCursor(0, 15 + (i*12));
              display.print(gamesRegistry[idx]->getName());
              display.setCursor(100, 15 + (i*12));
              display.print(highscores[idx]);
          }
      }
      display.display();
  }
}
