# Arduino Tetris (16x2 LCD)

A simplified version of **Tetris** that runs entirely on an **Arduino Uno**, using:
- a **16x2 LiquidCrystal LCD** (mounted vertically)
- **3 push buttons** (left, right, rotate)
- **3 LEDs** (score indicators)
- Optional: **Buzzer** for sound effects (not yet implemented)

---

## Hardware Overview

| Component | Purpose                    |
|----------|-----------------------------|
| LCD 16x2 | Displays the Tetris board   |
| Buttons  | Control movement + rotation |
| LEDs     | Show score (1 LED = 1 line) |
| Buzzer   | Sound feedback *(future)*   |

---

## How to Play

- Use the **left**, **right**, and **rotate** buttons to control the falling piece.
- Pieces fall automatically every 500ms.
- **Clear 3 lines** to win the game.
- If a new piece can't spawn, it's **game over**.
- If no lines are cleared in 15 seconds, **score resets**.

---

## Pin Configuration
Pin layout can be adjusted if needed — check `main.cpp`.

### LCD (LiquidCrystal)

RS = 12, E = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2


### Buttons

Left = Pin 6
Right = Pin 7
Rotate = Pin 8


### LEDs (score indicator)

LED_1 (score >= 1) → Pin 9
LED_2 (score >= 2) → Pin 10
LED_3 (score >= 3) → Pin 11

---

## Getting Started

1. **Upload** the code in `main.cpp` to an Arduino Uno.
2. **Wire up** the buttons, LEDs, and LCD according to the pin configuration above.
3. Open the **Serial Monitor** at `9600 baud` to view debug logs (e.g. rotations, spawn fails).
4. Play!

---

## File Structure
Uses mainly 2 files, main.cpp and pieces.h, which is inside lib. Pieces.h contains the logic oh the diferent peices available on the game and is used by main.cpp

## Credits
Made for fun and learning by:
- Marcel Marques 
- Laura Viana  
- Cauan Moronhe  
- Giovana Martins
