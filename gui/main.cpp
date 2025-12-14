#include "GameWindow.h"
#include "Menu.h"

int main() {
    // 1. On lance le menu et on récupère la config
    GameConfig config = runMenu();

    // 2. On lance le jeu avec cette config
    GameWindow game(config);
    game.run();

    return 0;
}