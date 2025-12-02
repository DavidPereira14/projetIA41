#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <vector>

// C'est cet include qui apporte la définition de Cell et PlayerColor.
// On ne doit donc PAS les redéfinir ici.
#include "PrologBridge.h"

class GameWindow
{
public:
    GameWindow();
    void run();

private:
    sf::RenderWindow window;

    // Notre pont vers Prolog
    PrologBridge prolog; // (On garde ça pour plus tard)

    // Cell est connu grâce à PrologBridge.h
    std::vector<Cell> boardState;
    std::vector<int> currentPath;

    // --- ÉLÉMENTS D'ÉTAT ---
    int selectedIndex;
    int targetIndex;
    sf::RectangleShape validateButton;

    bool isGameOver;       // Est-ce que le jeu est fini ?
    PlayerColor winner;    // Qui a gagné ?
    sf::Font font;         // La police d'écriture
    sf::Text victoryText;  // Le texte à afficher
    sf::Text subText;      // Sous-titre (ex: "Cliquer pour quitter")

    PlayerColor currentPlayerTurn;

    void drawBoard();
    void drawButton();
    void drawGameOver();
    void initMockBoard();
    void processEvents();
    void handleMouseClick(int x, int y);
    void playAITurn();
};

#endif