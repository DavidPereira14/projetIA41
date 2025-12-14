#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "PrologBridge.h"
#include "GameConfig.h"

// On utilise directement 'Cell' qui vient de PrologBridge.h

class GameWindow {
public:
    GameWindow(GameConfig config);
    void run();

private:
    // --- Moteur Graphique ---
    sf::RenderWindow window;
    sf::Font font;

    // --- UI ---
    sf::RectangleShape validateButton;
    sf::Text victoryText;
    sf::Text subText;

    // --- État du Jeu ---
    std::vector<Cell> boardState;

    std::vector<int> currentPath;
    PlayerColor currentPlayerTurn;
    bool isGameOver;
    PlayerColor winner;

    // --- Configuration ---
    PlayerType p1Type;
    PlayerType p2Type;

    // --- Interface Prolog ---
    PrologBridge prolog;

    // --- Méthodes ---
    void initMockBoard();
    void processEvents();
    void handleMouseClick(int mouseX, int mouseY);

    void updateGameLogic();
    void playAITurn(PlayerColor color);

    void drawBoard();
    void drawButton();
    void drawGameOver();
};

#endif // GAMEWINDOW_H