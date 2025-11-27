// Created by v05ax on 27/11/2025.

#include "GameWindow.h"
#include <iostream>

GameWindow::GameWindow()
        : window(sf::VideoMode(1100, 1200), "Pogo - IA41"),
          selectedIndex(-1),
          targetIndex(-1)
{
    // On initialise le plateau de test dès le lancement
    initMockBoard();

    // --- CONFIGURATION DU BOUTON VALIDER ---
    // Un rectangle Vert en bas à droite
    validateButton.setSize(sf::Vector2f(200, 80));
    validateButton.setPosition(850, 1025); // En bas à droite
    validateButton.setFillColor(sf::Color(50, 200, 50)); // Vert
    validateButton.setOutlineThickness(2);
    validateButton.setOutlineColor(sf::Color::White);
}

void GameWindow::initMockBoard() {
    boardState.resize(9);

    // Nettoyage complet
    for(auto& cell : boardState) {
        cell.stack.clear();
    }

    // --- Configuration Initiale Pogo ---
    // Noirs en haut
    boardState[0].stack = {BLACK, BLACK}; // Exemple simple
    boardState[1].stack = {BLACK, BLACK};
    boardState[2].stack = {BLACK, BLACK};

    // Blancs en bas
    boardState[6].stack = {WHITE, WHITE};
    boardState[7].stack = {WHITE, WHITE};
    boardState[8].stack = {WHITE, WHITE};
}

void GameWindow::run()
{
    while (window.isOpen())
    {
        processEvents();

        window.clear(sf::Color(50, 50, 50));

        drawBoard();
        drawButton(); // Cette fonction est maintenant définie plus bas !

        window.display();
    }
}

// --- AJOUT MANQUANT : La fonction pour dessiner le bouton ---
void GameWindow::drawButton() {
    // On dessine le bouton seulement si on a choisi une cible
    if (targetIndex != -1) {
        window.draw(validateButton);
    }
}

void GameWindow::drawBoard()
{
    float size = 300.f;
    float margeHaut = 100.0f;
    float margeGauche = 100.0f;

    float radius = 70.f;
    float epaisseur = 20.0f;
    float contour = 3.0f;

    for (int i = 0; i < 9; i++)
    {
        float x = (i % 3) * size + margeGauche;
        float y = (i / 3) * size + margeHaut;

        // 1. DESSINER LA CASE
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(100, 100, 100));

        // --- C'EST ICI QUE VOUS AVIEZ OUBLIÉ LA LOGIQUE DES COULEURS ---
        if (i == selectedIndex) {
            // DÉPART : Bordure VERTE
            rect.setOutlineThickness(-10.0f);
            rect.setOutlineColor(sf::Color::Green);
        }
        else if (i == targetIndex) {
            // ARRIVÉE : Bordure CYAN
            rect.setOutlineThickness(-10.0f);
            rect.setOutlineColor(sf::Color::Cyan);
        }
        else {
            // Case normale : Bordure NOIRE
            rect.setOutlineThickness(-5.0f);
            rect.setOutlineColor(sf::Color::Black);
        }

        window.draw(rect);

        // 2. DESSINER LES PIONS
        std::vector<PlayerColor>& pile = boardState[i].stack;

        for(size_t k = 0; k < pile.size(); k++) {

            float posX = x + size/2 - radius;
            float posY = y + size/2 - radius + 50 + (k * -epaisseur);

            sf::Color couleurFond;
            sf::Color couleurTrait;

            if (pile[k] == WHITE) {
                couleurFond = sf::Color::White;
                couleurTrait = sf::Color::Black;
            } else {
                couleurFond = sf::Color::Black;
                couleurTrait = sf::Color::White;
            }

            // A. LE BAS
            sf::CircleShape bas(radius);
            bas.setPosition(posX, posY + epaisseur);
            bas.setFillColor(couleurFond);
            bas.setOutlineThickness(contour);
            bas.setOutlineColor(couleurTrait);
            window.draw(bas);

            // B. LE CORPS
            sf::RectangleShape corps(sf::Vector2f(radius * 2, epaisseur));
            corps.setPosition(posX, posY + radius);
            corps.setFillColor(couleurFond);
            window.draw(corps);

            // C. BORDS VERTICAUX
            sf::RectangleShape bordGauche(sf::Vector2f(contour, epaisseur));
            bordGauche.setPosition(posX - contour, posY + radius);
            bordGauche.setFillColor(couleurTrait);
            window.draw(bordGauche);

            sf::RectangleShape bordDroit(sf::Vector2f(contour, epaisseur));
            bordDroit.setPosition(posX + (radius * 2), posY + radius);
            bordDroit.setFillColor(couleurTrait);
            window.draw(bordDroit);

            // D. LE HAUT
            sf::CircleShape haut(radius);
            haut.setPosition(posX, posY);
            haut.setFillColor(couleurFond);
            haut.setOutlineThickness(contour);
            haut.setOutlineColor(couleurTrait);
            window.draw(haut);
        }
    }
}

void GameWindow::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}


void GameWindow::handleMouseClick(int mouseX, int mouseY) {

    // 1. EST-CE UN CLIC SUR LE BOUTON VALIDER ?
    if (targetIndex != -1 && validateButton.getGlobalBounds().contains(mouseX, mouseY)) {
        std::cout << "ACTION : Validation du coup !" << std::endl;
        applyMove();
        return;
    }

    // 2. EST-CE UN CLIC SUR LA GRILLE ?
    float size = 300.f;
    float margeHaut = 100.0f;
    float margeGauche = 100.0f;

    if (mouseX < margeGauche || mouseX > margeGauche + (3 * size) ||
        mouseY < margeHaut || mouseY > margeHaut + (3 * size)) {
        selectedIndex = -1;
        targetIndex = -1;
        return;
    }

    int col = (mouseX - margeGauche) / size;
    int row = (mouseY - margeHaut) / size;
    int index = row * 3 + col;

    // --- MACHINE À ÉTATS DU CLIC ---

    // CAS A : Rien n'est sélectionné -> On choisit le DÉPART
    if (selectedIndex == -1) {
        if (boardState[index].stack.empty()) {
            std::cout << "Erreur : Case vide." << std::endl;
            return;
        }

        // RÈGLE : Doit être un pion NOIR (Mock)
        if (boardState[index].stack.back() == BLACK) {
            selectedIndex = index;
            std::cout << "Depart selectionne : Case " << index << std::endl;
        } else {
            std::cout << "Erreur : Ce n'est pas votre pion (Noir requis)." << std::endl;
        }
    }
        // CAS B : Départ déjà choisi -> On choisit la DESTINATION
    else if (selectedIndex != -1 && targetIndex == -1) {
        if (index == selectedIndex) {
            selectedIndex = -1;
            std::cout << "Selection annulee." << std::endl;
        } else {
            targetIndex = index;
            std::cout << "Destination choisie : Case " << index << std::endl;
        }
    }
        // CAS C : Tout est choisi -> Changement de destination
    else if (targetIndex != -1) {
        targetIndex = index;
        std::cout << "Nouvelle destination : Case " << index << std::endl;
    }
}

void GameWindow::applyMove() {
    if (selectedIndex == -1 || targetIndex == -1) return;

    std::vector<PlayerColor>& source = boardState[selectedIndex].stack;
    std::vector<PlayerColor>& dest = boardState[targetIndex].stack;

    // Déplacement de toute la pile (Mock)
    dest.insert(dest.end(), source.begin(), source.end());
    source.clear();

    std::cout << "Deplacement effectue de " << selectedIndex << " vers " << targetIndex << std::endl;

    selectedIndex = -1;
    targetIndex = -1;
}