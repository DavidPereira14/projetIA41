//
// Created by v05ax on 14/12/2025.
//

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// Définit les types de joueurs possibles
enum PlayerType { TYPE_HUMAN, TYPE_AI };

// Une structure pour stocker les choix du menu
struct GameConfig {
    PlayerType player1; // Noir
    PlayerType player2; // Blanc
};

#endif