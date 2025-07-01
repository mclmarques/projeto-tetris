#include <Arduino.h>
#include <pieces.h>
#include <LiquidCrystal.h>
/*
 * Tetris Game — Arduino Uno
 * Display: 16x2 LCD mounted vertically (2 columns x 16 rows)
 * Inputs: 3 buttons (left, right, rotate)
 * Author: Equipe formada por:
-> Marcel Marques
-> Laura Viana
-> Cauan Moronhe
-> Giovana Martins

 * Notes:
 * - Pin layout for LCD must be defined by hardware team
 * - Uses LiquidCrystal library for LCD rendering
 * - Buttons use INPUT_PULLUP mode (active LOW)
 * - Score resets if no line is cleared in 15 seconds
 * - Win condition: clear 3 lines
 * - Game ends if top row is blocked on spawn
 * - Buzzer hasn't been implemnted. Check number of avail pins 
 */


// Buttons (active LOW w/ INPUT_PULLUP)
const byte BTN_LEFT = 6;
const byte BTN_RIGHT = 7;
const byte BTN_ROTATE = 8;

// LEDs
const byte LED_1   = 9;   
const byte LED_2    = 10;  
const byte LED_3   = 11;  

//TODO: Buzzer
//const byte buzzer = 12;

//Display. TODO: ALTER AND CONFIGURE FOR THE CORRECT PINS
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


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
  byte rotation;             // which of the 4 orientations (0–3). CURRENTLY UNUSED as rotatinons are done on a separate funciton using matrix
  int x;                     // horizontal position (can be negative)
  int y;                     // vertical position (can be negative)
  byte width;
  byte height;
};
ActivePiece currentPiece;

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

  // Copy the shape into the working RAM buffer
  memcpy(currentPiece.shape, def->shape, sizeof(currentPiece.shape));
  currentPiece.width = def->width;
  currentPiece.height = def->height;

  // Center horizontally
  currentPiece.x = (FIELD_WIDTH - currentPiece.width) / 2;
  currentPiece.y = -currentPiece.height;

  // Check if the new piece is immediately blocked
  PieceDef temp;
  memcpy(temp.shape, currentPiece.shape, sizeof(temp.shape));
  temp.width = currentPiece.width;
  temp.height = currentPiece.height;

  if (checkCollision(currentPiece.x, currentPiece.y, &temp)) {
    gameState = STATE_LOST;
    Serial.println("GAME OVER: piece can't spawn");
    return;
  }

  Serial.print("Spawned piece: ");
  Serial.println(index);
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
 * @brief Rotates a square matrix 90 degrees clockwise.
 * 
 * @param src  The original matrix (4x4) to rotate.
 * @param dest The rotated result (4x4), written into this array.
 */
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
  if (digitalRead(BTN_ROTATE) == LOW) {
    tryRotate();
  }
}

/**
 * @brief Locks the current falling piece into the field, clears full rows,
 *        updates score/timer, and either ends the game or spawns a new piece.
 */
void lockPiece() {
  // 1) Stamp currentPiece into the field
  for (byte r = 0; r < PIECE_SIZE; r++) {
    for (byte c = 0; c < PIECE_SIZE; c++) {
      if (currentPiece.shape[r][c] == 0) continue;
      int fy = currentPiece.y + r;
      int fx = currentPiece.x + c;
      // Only write if within visible bounds
      if (fy >= 0 && fy < FIELD_HEIGHT && fx >= 0 && fx < FIELD_WIDTH) {
        field[fy][fx] = FILLED;
      }
    }
  }

  // 2) Check every row for fullness, clear & shift down
  for (int row = 0; row < FIELD_HEIGHT; row++) {
    bool full = true;
    for (int col = 0; col < FIELD_WIDTH; col++) {
      if (field[row][col] == EMPTY) {
        full = false;
        break;
      }
    }
    if (full) {
      // Row is full → increase score & reset timer
      score++;
      lastScoreTime = millis();

      // Shift everything above this row down by one
      for (int y = row; y > 0; y--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          field[y][x] = field[y-1][x];
        }
      }
      // Clear top row
      for (int x = 0; x < FIELD_WIDTH; x++) {
        field[0][x] = EMPTY;
      }

      // Stay on this row index to catch cascading clears
      row--;
    }
  }

  // 3) Win check
  if (score >= WIN_SCORE) {
    gameState = STATE_WON;
    Serial.println("You win!");
    return;
  }

  // 4) Otherwise spawn a new piece and continue
  spawnRandomPiece();
}



// ─────────────────────────────────────────────
// Helper: clear the playfield
// ─────────────────────────────────────────────
void initField() {
  for (byte r = 0; r < FIELD_HEIGHT; r++) {
    for (byte c = 0; c < FIELD_WIDTH; c++) {
      field[r][c] = EMPTY;
    }
  }
}

// ─────────────────────────────────────────────
// Helper: reset score if timeout exceeded
// ─────────────────────────────────────────────
void checkScoreTimeout() {
  if (score > 0 && (millis() - lastScoreTime) > SCORE_TIMEOUT) {
    score = 0;
    lastScoreTime = millis();
    Serial.println("Score reset due to timeout");
    digitalWrite(LED_1, score >= 1 ? HIGH : LOW);
    digitalWrite(LED_2, score >= 2 ? HIGH : LOW);
    digitalWrite(LED_3, score >= 3 ? HIGH : LOW);
  }
}

// ─────────────────────────────────────────────
// Helper: control LEDs
// ─────────────────────────────────────────────
void updateScoreLEDs() {
  digitalWrite(LED_1, score >= 1 ? HIGH : LOW);
  digitalWrite(LED_2, score >= 2 ? HIGH : LOW);
  digitalWrite(LED_3, score >= 3 ? HIGH : LOW);
}


/**
 * @brief Renders the entire game field to the LCD.
 * 
 * The LCD is mounted vertically, so each row in the Tetris field maps
 * to a column on the screen. Because the LCD is 16 rows high × 2 cols wide,
 * we treat this as a rotated board.
 * 
 * LCD coordinates: (column = field row, row = field column)
 */
void updateDisplay() {
  // Clear previous frame
  lcd.clear(); // Note: causes flicker, can be optimized if needed

  // For every playfield row (0–15) and column (0–1):
  for (byte r = 0; r < FIELD_HEIGHT; r++) {
    for (byte c = 0; c < FIELD_WIDTH; c++) {
      // Determine what to draw
      bool drawBlock = false;

      // 1) Does the active piece occupy (r,c)?
      int pr = r - currentPiece.y;
      int pc = c - currentPiece.x;
      if (pr >= 0 && pr < PIECE_SIZE
          && pc >= 0 && pc < PIECE_SIZE
          && currentPiece.shape[pr][pc] == 1) {
        drawBlock = true;
      }
      // 2) Otherwise, is the static field occupied?
      else if (field[r][c] == FILLED) {
        drawBlock = true;
      }

      // Map (r,c) → LCD coordinate: column = r, row = c
      lcd.setCursor(r, c);

      // 0xFF is the “full block” character in the default charset
      lcd.write(drawBlock ? byte(0xFF) : byte(' '));
    }
  }
}


void setup() {
    Serial.begin(9600);
  //TODO: VERIFY CONDIGURATION

  lcd.begin(16, 2);
  // buttons
  pinMode(BTN_LEFT,   INPUT_PULLUP);
  pinMode(BTN_RIGHT,  INPUT_PULLUP);
  pinMode(BTN_ROTATE, INPUT_PULLUP);

  //LEDs
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2,  OUTPUT);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2,  LOW);
  digitalWrite(LED_3, LOW);

  // TODO: check if buzzer has pins left and enable this section
  //pinMode(buzzer, OUTPUT);

  // clear the board & start the first piece
  initField();
  updateScoreLEDs();  // make sure LEDs reflect score = 0 on boot
  lastScoreTime = millis();
  spawnRandomPiece();
}

void loop() {
  // Check if game is over, and stops the code. To play again, restart the board
   if (gameState == STATE_WON) {
    Serial.println("YOU WIN!"); 
    while (true) {}  // halt
  }
  if (gameState == STATE_LOST) {
    Serial.println("GAME OVER");
    while (true) {}  // halt
  }

  handlePieceControl();

  // Gravity: try to fall every 500ms
  static unsigned long lastFall = 0;
  unsigned long now = millis();
  if (now - lastFall >= 500) {
    lastFall = now;
    // attempt to move down. If blocked, lock the piece
    int newY = currentPiece.y + 1;
    PieceDef test = {}; // Temporary shape for collision checking
    memcpy(test.shape, currentPiece.shape, sizeof(test.shape));
    test.width  = currentPiece.width;
    test.height = currentPiece.height;

    if (!checkCollision(currentPiece.x, newY, &test)) {
      currentPiece.y = newY; // Move piece down
    } else {
      lockPiece(); // Piece landed, lock it and spawn a new one
    }
  }
  checkScoreTimeout();

  updateDisplay();
}
