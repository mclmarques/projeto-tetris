#include <Arduino.h>
#include <pieces.h>

const byte FIELD_WIDTH    = 2;       // 2 columns
const byte FIELD_HEIGHT   = 16;      // 16 rows
const byte WIN_SCORE      = 3;       // points needed to win
const unsigned long SCORE_TIMEOUT = 15000UL;  // 15 seconds (ms)

enum CellState : byte {
  EMPTY  = 0,
  FILLED = 1
};

enum GameState : byte {
  STATE_PLAYING,
  STATE_WON,
  STATE_LOST
};

// A 2D array of rows × cols. Each cell is either EMPTY (0) or FILLED (1).
CellState field[FIELD_HEIGHT][FIELD_WIDTH];

// Stores current falling piece’s shape, rotation, and position on field.
struct ActivePiece {
  const PieceDef* def;  // pointer to static piece definition
  byte rotation;             // which of the 4 orientations (0–3)
  int x;                     // horizontal position (can be negative)
  int y;                     // vertical position (can be negative)
};

ActivePiece currentPiece;

//Score & game state 
byte score = 0;                     // score counter (0–3 max)
unsigned long lastScoreTime = 0;   // millis() of last point
GameState gameState = STATE_PLAYING;


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
