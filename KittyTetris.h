#ifndef KITTYTETRIS_H
#define KITTYTETRIS_H

#include <vector>
#include <cstdlib>
#include <cstring>

typedef unsigned int u32;

class KittyTetris {
private:
    static const int WIDTH = 10;
    static const int HEIGHT = 20;
    int board[HEIGHT][WIDTH];
    u32 colorBoard[HEIGHT][WIDTH];

    u32 NEON_COLORS[7] = {
        0x00FFFFFF, // Turkuaz
        0x00FF00FF, // Yesil
        0xFF00FFFF, // Magenta
        0xFFFF00FF, // Sari
        0xFF3333FF, // Kirmizi
        0x3333FFFF, // Mavi
        0xFF9900FF  // Turuncu
    };

    std::vector<std::vector<std::vector<int>>> SHAPES = {
        {{1, 1, 1, 1}},
        {{1, 0, 0}, {1, 1, 1}},
        {{0, 0, 1}, {1, 1, 1}},
        {{1, 1}, {1, 1}},
        {{0, 1, 1}, {1, 1, 0}},
        {{0, 1, 0}, {1, 1, 1}},
        {{1, 1, 0}, {0, 1, 1}}
    };

    std::vector<std::vector<int>> currentPiece;
    int currentPieceIndex;
    int pieceX, pieceY;
    
    int score;
    int linesCleared;
    int level;
    int gameSpeedFrames; // Dusme hizi (kare cinsinden)
    bool isGameOver;

public:
    KittyTetris() {
        resetGame();
    }

    void resetGame() {
        std::memset(board, 0, sizeof(board));
        std::memset(colorBoard, 0, sizeof(colorBoard));
        score = 0;
        linesCleared = 0;
        level = 1;
        gameSpeedFrames = 45; // Yaklasik 60 FPS'te yarim saniyeden biraz fazla
        isGameOver = false;
        spawnPiece();
    }

    void spawnPiece() {
        currentPieceIndex = std::rand() % 7;
        currentPiece = SHAPES[currentPieceIndex];
        pieceX = WIDTH / 2 - currentPiece[0].size() / 2;
        pieceY = 0;

        if (checkCollision(pieceX, pieceY, currentPiece)) {
            isGameOver = true;
        }
    }

    bool checkCollision(int nextX, int nextY, const std::vector<std::vector<int>>& piece) {
        for (size_t r = 0; r < piece.size(); r++) {
            for (size_t c = 0; c < piece[r].size(); c++) {
                if (piece[r][c] != 0) {
                    int boardX = nextX + c;
                    int boardY = nextY + r;
                    if (boardX < 0 || boardX >= WIDTH || boardY >= HEIGHT) return true;
                    if (boardY >= 0 && board[boardY][boardX] != 0) return true;
                }
            }
        }
        return false;
    }

    void moveLeft() {
        if (!isGameOver && !checkCollision(pieceX - 1, pieceY, currentPiece)) pieceX--;
    }

    void moveRight() {
        if (!isGameOver && !checkCollision(pieceX + 1, pieceY, currentPiece)) pieceX++;
    }

    void moveDown() {
        if (isGameOver) return;
        
        if (!checkCollision(pieceX, pieceY + 1, currentPiece)) {
            pieceY++;
        } else {
            mergePiece();
            spawnPiece();
        }
    }

    void rotate() {
        if (isGameOver) return;

        int r = currentPiece.size();
        int c = currentPiece[0].size();
        std::vector<std::vector<int>> rotated(c, std::vector<int>(r));
        
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                rotated[j][r - 1 - i] = currentPiece[i][j];
            }
        }
        
        if (!checkCollision(pieceX, pieceY, rotated)) {
            currentPiece = rotated;
        }
    }

    void mergePiece() {
        for (size_t r = 0; r < currentPiece.size(); r++) {
            for (size_t c = 0; c < currentPiece[r].size(); c++) {
                if (currentPiece[r][c] != 0 && pieceY + (int)r >= 0) {
                    board[pieceY + r][pieceX + c] = 1;
                    colorBoard[pieceY + r][pieceX + c] = NEON_COLORS[currentPieceIndex];
                }
            }
        }
        checkLines();
    }

    void checkLines() {
        int comboCount = 0;
        for (int r = HEIGHT - 1; r >= 0; r--) {
            bool isFull = true;
            for (int c = 0; c < WIDTH; c++) {
                if (board[r][c] == 0) { isFull = false; break; }
            }
            if (isFull) {
                comboCount++;
                linesCleared++;
                for (int row = r; row > 0; row--) {
                    std::memcpy(board[row], board[row - 1], sizeof(board[0]));
                    std::memcpy(colorBoard[row], colorBoard[row - 1], sizeof(colorBoard[0]));
                }
                std::memset(board[0], 0, sizeof(board[0]));
                std::memset(colorBoard[0], 0, sizeof(colorBoard[0]));
                r++; 
            }
        }

        if (comboCount == 1) score += 100 * level;
        else if (comboCount == 2) score += 300 * level;
        else if (comboCount == 3) score += 500 * level;
        else if (comboCount == 4) score += 800 * level;

        int newLevel = (linesCleared / 5) + 1;
        if (newLevel > level) {
            level = newLevel;
            gameSpeedFrames = (60 - (level * 4) > 10) ? 60 - (level * 4) : 10;
        }
    }

    // Getters
    u32 getColorAt(int r, int c) const { return colorBoard[r][c]; }
    bool getIsGameOver() const { return isGameOver; }
    int getScore() const { return score; }
    int getLevel() const { return level; }
    int getLinesCleared() const { return linesCleared; }
    int getGameSpeedFrames() const { return gameSpeedFrames; }
    int getPieceX() const { return pieceX; }
    int getPieceY() const { return pieceY; }
    u32 getCurrentPieceColor() const { return NEON_COLORS[currentPieceIndex]; }
    const std::vector<std::vector<int>>& getCurrentPiece() const { return currentPiece; }
};

#endif