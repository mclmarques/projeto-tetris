#include <Arduino.h>
#include <pieces.h>

//Buttons pins map
const byte BTN_LEFT = 2;
const byte BTN_RIGHT = 3;
const byte BTN_DOWN = 4;
const byte BTN_ROTATE = 5;


//Gameboard configs
const byte FIELD_WIDTH    = 2;       // 2 columns
const byte FIELD_HEIGHT   = 16;      // 16 rows
enum CellState : byte {
  EMPTY  = 0,
  FILLED = 1
};
// A 2D array of rows × cols. Each cell is either EMPTY (0) or FILLED (1).
CellState field[FIELD_HEIGHT][FIELD_WIDTH];


//Score & game state 
enum GameState : byte {
  STATE_PLAYING,
  STATE_WON,
  STATE_LOST
};
byte score = 0;                     // score counter (0–3 max)
const byte WIN_SCORE      = 3;       // points needed to win
unsigned long lastScoreTime = 0;   // millis() of last point
GameState gameState = STATE_PLAYING;
const unsigned long SCORE_TIMEOUT = 15000UL;  // 15 seconds (ms)



// Stores current falling piece’s shape, rotation, and position on field.
struct ActivePiece {
  const PieceDef* def;  // pointer to static piece definition
  byte rotation;             // which of the 4 orientations (0–3)
  int x;                     // horizontal position (can be negative)
  int y;                     // vertical position (can be negative)
};
ActivePiece currentPiece;

void spawnRandomPiece() {
  byte index = random(NUM_PIECES);
  currentPiece.def = &PIECES[index];
  currentPiece.rotation = 0;

  // Center horizontally
  currentPiece.x = (FIELD_WIDTH - currentPiece.def->width) / 2;

  // Start just above the visible field (so tall pieces don't crash instantly)
  currentPiece.y = -currentPiece.def->height;

  Serial.print("Spawned piece: ");
  Serial.println(index);
}

bool checkCollision(int x, int y, const PieceDef* def) {
  for (byte r = 0; r < PIECE_SIZE; r++) {
    for (byte c = 0; c < PIECE_SIZE; c++) {
      if (def->shape[r][c] == 0) continue;

      int fieldX = x + c;
      int fieldY = y + r;

      if (fieldX < 0 || fieldX >= FIELD_WIDTH ||
          fieldY >= FIELD_HEIGHT) return true;  // out of bounds

      if (fieldY >= 0 && field[fieldY][fieldX] == FILLED)
        return true;  // collides with landed block
    }
  }
  return false;
}

void tryMove(int dx, int dy) {
  int newX = currentPiece.x + dx;
  int newY = currentPiece.y + dy;

  if (!checkCollision(newX, newY, currentPiece.def)) {
    currentPiece.x = newX;
    currentPiece.y = newY;
  }
}

void handlePieceControl() {
  if (digitalRead(BTN_LEFT) == LOW) {
    tryMove(-1, 0);  // Move left
  }
  if (digitalRead(BTN_RIGHT) == LOW) {
    tryMove(1, 0);   // Move right
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    tryMove(0, 1);   // Soft drop
  }
  if (digitalRead(BTN_ROTATE) == LOW) {
    //TODO: tryRotate();
  }
}



void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
