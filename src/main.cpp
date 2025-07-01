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
const byte WIN_SCORE = 3;       // points needed to win
unsigned long lastScoreTime = 0;   // millis() of last point
GameState gameState = STATE_PLAYING;
const unsigned long SCORE_TIMEOUT = 15000UL;  // 15 seconds (ms)



// Stores current falling piece’s shape, rotation, and position on field.
struct ActivePiece {
  byte shape[PIECE_SIZE][PIECE_SIZE];  // mutable copy from one of the available pieces on the pieces.h
  byte rotation;             // which of the 4 orientations (0–3)
  int x;                     // horizontal position (can be negative)
  int y;                     // vertical position (can be negative)
  byte width;
  byte height;
};
ActivePiece currentPiece;

/**
 * @brief Spawns a new random Tetris piece at the top of the field.
 * 
 * Selects a random piece from the PIECES[] array, assigns it to the 
 * global currentPiece variable, and places it horizontally centered 
 * and vertically just above the visible playfield.
 * 
 * This function should be called after a piece has locked or at game start.
 */
void spawnRandomPiece() {
  byte index = random(NUM_PIECES);
  const PieceDef* def = &PIECES[index];

  memcpy(currentPiece.shape, def->shape, sizeof(currentPiece.shape));
  currentPiece.width = def->width;
  currentPiece.height = def->height;

  currentPiece.x = (FIELD_WIDTH - currentPiece.width) / 2;
  currentPiece.y = -currentPiece.height;
}


/**
 * @brief Checks if a piece placed at a specific position would collide
 *        with the field boundaries or filled blocks.
 * 
 * @param x X-coordinate on the field (column position for the piece's top-left corner).
 * @param y Y-coordinate on the field (row position for the piece's top-left corner).
 * @param def Pointer to the piece definition to check against.
 * @return true If any part of the piece would be out of bounds or overlap filled blocks.
 * @return false If the piece can be placed safely at the given position.
 * 
 * This function loops over all 1s in the piece shape matrix and calculates
 * where they would fall on the field based on the input (x, y). If any block 
 * would fall outside bounds or onto a filled cell, it returns true (collision).
 */
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

/**
 * @brief Attempts to move the current piece by a delta offset (dx, dy).
 * 
 * @param dx Change in the X direction (e.g., -1 for left, +1 for right).
 * @param dy Change in the Y direction (e.g., +1 for downward movement).
 * 
 * If the move would result in a collision (checked via checkCollision),
 * the move is rejected and the piece remains in place.
 */
void tryMove(int dx, int dy) {
  int newX = currentPiece.x + dx;
  int newY = currentPiece.y + dy;

  // Build a temporary PieceDef from the current piece
  PieceDef temp;
  memcpy(temp.shape, currentPiece.shape, sizeof(temp.shape));
  temp.width = currentPiece.width;
  temp.height = currentPiece.height;

  if (!checkCollision(newX, newY, &temp)) {
    currentPiece.x = newX;
    currentPiece.y = newY;
  }
}


/**
 * @brief Reads the current button inputs and applies movement or rotation
 *        to the falling piece.
 * 
 * Input pins must be defined for left, right, down, and rotate. When a button
 * is pressed (active LOW), the corresponding action is triggered:
 * 
 * - Left: Move piece one cell to the left
 * - Right: Move piece one cell to the right
 * - Down: Move piece one cell downward (soft drop)
 * - Rotate: TODO – rotate the current piece
 * 
 * This function should be called repeatedly in the main loop.
 */
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

void rotateMatrix(const byte src[PIECE_SIZE][PIECE_SIZE], byte dest[PIECE_SIZE][PIECE_SIZE]) {
  for (byte r = 0; r < PIECE_SIZE; r++) {
    for (byte c = 0; c < PIECE_SIZE; c++) {
      dest[c][PIECE_SIZE - 1 - r] = src[r][c];
    }
  }
}


void tryRotate() {
  byte rotated[PIECE_SIZE][PIECE_SIZE];
  rotateMatrix(currentPiece.shape, rotated);

  // New width and height are swapped
  byte newWidth = currentPiece.height;
  byte newHeight = currentPiece.width;

  // Build a temp PieceDef to use with collision checker
  PieceDef temp;
  memcpy(temp.shape, rotated, sizeof(rotated));
  temp.width = newWidth;
  temp.height = newHeight;

  if (!checkCollision(currentPiece.x, currentPiece.y, &temp)) {
    memcpy(currentPiece.shape, rotated, sizeof(rotated));
    currentPiece.width = newWidth;
    currentPiece.height = newHeight;
    Serial.println("Rotated successfully");
  } else {
    Serial.println("Rotation blocked");
  }
}






void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
