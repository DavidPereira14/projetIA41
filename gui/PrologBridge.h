//
// Created by v05ax on 27/11/2025.
//

#ifndef PROLOGBRIDGE_H
#define PROLOGBRIDGE_H

#include <string>
#include <SWI-cpp.h>
#include <vector>

enum PlayerColor { EMPTY, WHITE, BLACK };

struct Cell {
    std::vector<PlayerColor> stack;
};

// Ajoutez cette structure au début (après Cell)
struct MoveInfo {
    int start;
    int end;
    int numPieces;
};

class PrologBridge
{
public:
    PrologBridge();
    ~PrologBridge();

    // Récupère le plateau initial sous forme de vecteur de piles
    std::vector<std::string> getInitialBoard();

    // Joue un coup (from = case départ, count = nb pièces, path = liste indices des cases)
    bool isValidMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player);

    std::vector<Cell> executeMove(const std::vector<Cell>& board, int start, int end, int numPieces, PlayerColor player);

    MoveInfo getAIMove(const std::vector<Cell>& board, PlayerColor player);

    PlayerColor getWinner(const std::vector<Cell>& board);

    // Récupère le plateau courant
    std::vector<std::string> getBoard();

private:
    // Initialise Prolog (appelée automatiquement dans le constructeur)
    void initProlog();


};

#endif

