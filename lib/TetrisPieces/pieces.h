// tetris_pieces.h
#include <Arduino.h>   
#ifndef TETRIS_PIECES_H
#define TETRIS_PIECES_H

// Each piece is defined in a 4x4 matrix (max size for Tetris piece)
// LCD is 16x2 characters -> treated as vertical grid (2 cols x 16 rows)

#define PIECE_SIZE 4

struct Piece {
    byte shape[PIECE_SIZE][PIECE_SIZE];
    byte width;
    byte height;
};

const Piece PIECES[] = {
    // I-Piece (vertical line)
    {
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 0, 0, 0}
        },
        1, 4
    },

    // O-Piece (square)
    {
        {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        2, 2
    },

    // T-Piece
    {
        {
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        3, 2
    },

    // L-Piece
    {
        {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        2, 3
    },

    // Z-Piece
    {
        {
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        3, 2
    }
};

const byte NUM_PIECES = sizeof(PIECES) / sizeof(Piece);

#endif
