#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "PrologBridge.h"

enum PlayerColor { EMPTY, WHITE, BLACK };

struct Cell {
    std::vector<PlayerColor> stack;
};

class GameWindow
{
public:
    GameWindow();
    void run();

private:
    sf::RenderWindow window;
    // PrologBridge prolog; // (On le garde pour plus tard)

    std::vector<Cell> boardState;

    // --- NOUVEAUX ÉLÉMENTS D'ÉTAT ---
    int selectedIndex;  // -1 si rien, 0-8 si case choisie
    int targetIndex;    // -1 si rien, 0-8 si destination choisie
    sf::RectangleShape validateButton; // Le bouton graphique

    void drawBoard();
    void drawButton();
    void initMockBoard();
    void processEvents();
    void handleMouseClick(int x, int y);
    void applyMove(); // Pour effectuer le déplacement une fois validé
};

#endif