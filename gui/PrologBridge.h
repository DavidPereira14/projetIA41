//
// Created by v05ax on 27/11/2025.
//

#ifndef PROLOGBRIDGE_H
#define PROLOGBRIDGE_H

#include <string>
#include <SWI-cpp.h>
#include <vector>

class PrologBridge
{
public:
    PrologBridge();
    ~PrologBridge();

    // Récupère le plateau initial sous forme de vecteur de piles
    std::vector<std::string> getInitialBoard();

    // Joue un coup (from = case départ, count = nb pièces, path = liste indices des cases)
    bool playMove(int from, int count, const std::vector<int>& path);

    // Récupère le plateau courant
    std::vector<std::string> getBoard();

private:
    // Initialise Prolog (appelée automatiquement dans le constructeur)
    void initProlog();
};

#endif

