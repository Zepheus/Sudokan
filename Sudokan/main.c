/*  Author: Cedric Van Goethem
 *  Description: A basic sudoku solver implemented through backtracking
 *  You may redistribute this file with proper credits. Commercial use allowed
 */

#define _CRT_SECURE_NO_WARNINGS /* Microsoft please shut up... */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void print_puzzle(uint8_t **data) {
    uint8_t i, j;

    for(i = 0; i < 9; ++i) {
        if(i && !(i % 3))
            printf("\n\n");
        else
            printf("\n");

        for(j = 0; j < 9; ++j) {
            if(j && !(j % 3))
                printf("\t\t");
            printf("%2d ", data[i][j] & 0xF);
        }
    }
    printf("\n");
}

uint8_t is_valid_move(uint8_t **data, uint8_t row, uint8_t column, uint8_t val) {
    uint8_t i, j, sq_row, sq_column;

    /* Check if there's not a piece on it yet */
    if(data[row][column] != 0)
        return 0;

    /* Check on the row or column */
    for (i = 0; i < 9; ++i) {
        if((data[row][i] & 0xF) == val || (data[i][column] & 0xF) == val)
            return 0;
    }

    /* Check in the squares of 9 */
    sq_row = row / 3;
    sq_column = column / 3;
    for(i = sq_row * 3; i < (sq_row + 1) * 3; ++i){
        for(j = sq_column * 3; j < (sq_column + 1) * 3; ++j) {
            if((data[i][j] & 0xF) == val)
                return 0;
        }
    }

    return 1;
}

uint64_t steps = 0;
uint8_t solve_piece(uint8_t **data, uint8_t row, uint8_t column) {
    uint8_t i;

    ++steps;
    if(column == 9) {
        if(row == 8)
            return 1; /* Solved the puzzle */
        else {
            return solve_piece(data, row + 1, 0); /* Advance to the next row */
        }
    } else if(data[row][column] & 0x80) { /* Static piece */
        return solve_piece(data, row, column + 1);
    } else {
        for(i = 1; i <= 9; ++i) {
            if(is_valid_move(data, row, column, i)) {
                data[row][column] = i; /* Place piece */
                if(solve_piece(data, row, column + 1)) { /* Advance to next column */
                    return 1;
                }
                data[row][column] = 0; /* Failed => remove piece */
            }
        }
        return 0;
    }
}

uint8_t solve_puzzle(char *filename) {
    FILE *file;
    uint8_t f[9*9], *data[9], i, j;
    char line[128];

    for(i = 0; i < 9; ++i)
        data[i] = f + i*9;

    file = fopen(filename, "r");
    if(!file) {
        fprintf(stderr, "Could not open file:%s\n", filename);
        return 0;
    }

    /* Read puzzle data */
    for(i = 0; i < 9; ++i) {
        if(!fgets(line,sizeof(line),file)){
            fprintf(stderr, "Failed to read all lines in input.\n", filename);
            fclose(file);
            return 0;
        }

        for(j = 0; j < 9; ++j) {
            if(line[j] == 'x') {
                data[i][j] = 0;
            } else {
                data[i][j] = (line[j] - '0') | 0x80; /* we flag it as static */
            }
        }
    }
    fclose(file);

    /* Solve the puzzle */
    if(solve_piece(data, 0, 0)) {
        printf("Solution (found in %lu steps) :\n", steps);
        print_puzzle(data);
        return 1;
    } else
        return 0;
}

int main(int argcnt, char** args) {
    printf("Sudoku solver v1.0 - Cedric Van Goethem\n");
    if(argcnt <= 1){
        fprintf(stderr, "Usage: Sudokan [file].txt\n");
        return 1;
    }

    if(!solve_puzzle(args[1]))
        printf("Could not solve puzzle.\n");

    system("pause");
}