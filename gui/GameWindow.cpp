// Created by v05ax on 27/11/2025.

#include "GameWindow.h"
#include <iostream>

GameWindow::GameWindow()
        : window(sf::VideoMode(1100, 1100), "Pogo - IA41"),
          selectedIndex(-1),
          targetIndex(-1),
          currentPlayerTurn(BLACK),
          isGameOver(false),
          winner(EMPTY)
{
    window.setFramerateLimit(60);
    initMockBoard();
    currentPath.clear();

    // --- CONFIGURATION DU BOUTON VALIDER ---
    validateButton.setSize(sf::Vector2f(200, 80));
    validateButton.setPosition(850, 1005 );
    validateButton.setFillColor(sf::Color(50, 200, 50));
    validateButton.setOutlineThickness(2);
    validateButton.setOutlineColor(sf::Color::White);

    // --- CONFIGURATION TEXTE DE FIN (NOUVEAU) ---
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Erreur: Police arial.ttf introuvable." << std::endl;
    }

    victoryText.setFont(font);
    victoryText.setCharacterSize(80);
    victoryText.setOutlineThickness(4);

    subText.setFont(font);
    subText.setString("(Cliquer pour quitter)");
    subText.setCharacterSize(30);
    subText.setFillColor(sf::Color::Yellow);
    subText.setOutlineColor(sf::Color::Black);
    subText.setOutlineThickness(2);
}

void GameWindow::initMockBoard() {
    boardState.resize(9);
    for(auto& cell : boardState) cell.stack.clear();

    // Configuration Initiale
    boardState[0].stack = {BLACK, BLACK};
    boardState[1].stack = {BLACK, BLACK};
    boardState[2].stack = {BLACK, BLACK};

    boardState[6].stack = {WHITE, WHITE};
    boardState[7].stack = {WHITE, WHITE};
    boardState[8].stack = {WHITE, WHITE};
}

void GameWindow::run() {
    while (window.isOpen()) {
        processEvents();
        window.clear(sf::Color(50, 50, 50));

        drawBoard();
        drawButton();

        // Afficher l'écran de fin si nécessaire
        if (isGameOver) {
            drawGameOver();
        }

        window.display();

        // --- VERIFICATION VICTOIRE ---
        if (!isGameOver) {
            PlayerColor w = prolog.getWinner(boardState);
            if (w != EMPTY) {
                isGameOver = true;
                winner = w;

                // Configurer le message
                if (winner == WHITE) {
                    victoryText.setString("L'IA (BLANC)\nA GAGNE !");
                    victoryText.setFillColor(sf::Color(200, 200, 255));
                    victoryText.setOutlineColor(sf::Color::Black);
                } else {
                    victoryText.setString("VOUS (NOIR)\nAVEZ GAGNE !");
                    victoryText.setFillColor(sf::Color(100, 255, 100));
                    victoryText.setOutlineColor(sf::Color::Black);
                }

                // Centrage
                sf::FloatRect tr = victoryText.getLocalBounds();
                victoryText.setOrigin(tr.left + tr.width/2.0f, tr.top + tr.height/2.0f);
                victoryText.setPosition(550, 550); // Centre de 1100x1100

                sf::FloatRect sr = subText.getLocalBounds();
                subText.setOrigin(sr.left + sr.width/2.0f, sr.top + sr.height/2.0f);
                subText.setPosition(550, 700);
            }
        }

        // --- TOUR IA ---
        if (!isGameOver && currentPlayerTurn == WHITE) {
            sf::sleep(sf::milliseconds(500));
            playAITurn();
        }
    }
}

void GameWindow::playAITurn() {
    std::cout << "--- TOUR DE L'IA (BLANC) ---" << std::endl;
    MoveInfo move = prolog.getAIMove(boardState, WHITE);

    if (move.start != -1) {
        boardState = prolog.executeMove(boardState, move.start, move.end, move.numPieces, WHITE);
        std::cout << "-> IA a joue !" << std::endl;
    } else {
        std::cout << "-> IA bloquee." << std::endl;
    }
    currentPlayerTurn = BLACK;
}

void GameWindow::drawGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(1100, 1100));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    window.draw(victoryText);
    window.draw(subText);
}

void GameWindow::drawButton() {
    // On affiche le bouton si on a tracé un chemin valide ET que le jeu n'est pas fini
    if (currentPath.size() >= 2 && !isGameOver) {
        window.draw(validateButton);
    }
}

void GameWindow::drawBoard() {
    float size = 300.f; float margeHaut = 100.0f; float margeGauche = 100.0f;
    float radius = 70.f; float epaisseur = 20.0f; float contour = 3.0f;

    for (int i = 0; i < 9; i++) {
        float x = (i % 3) * size + margeGauche;
        float y = (i / 3) * size + margeHaut;

        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y); rect.setFillColor(sf::Color(100, 100, 100));

        // Couleurs du chemin
        auto it = std::find(currentPath.begin(), currentPath.end(), i);
        if (it != currentPath.end()) {
            if (it == currentPath.begin()) {
                rect.setOutlineThickness(-10.0f); rect.setOutlineColor(sf::Color::Green);
            } else {
                rect.setOutlineThickness(-10.0f); rect.setOutlineColor(sf::Color::Cyan);
            }
        } else {
            rect.setOutlineThickness(-5.0f); rect.setOutlineColor(sf::Color::Black);
        }
        window.draw(rect);

        // Pions
        std::vector<PlayerColor>& pile = boardState[i].stack;
        for(size_t k = 0; k < pile.size(); k++) {
            float posX = x + size/2 - radius;
            float posY = y + size/2 - radius + 50 + (k * -epaisseur);
            sf::Color cFond = (pile[k] == WHITE) ? sf::Color::White : sf::Color::Black;
            sf::Color cTrait = (pile[k] == WHITE) ? sf::Color::Black : sf::Color::White;

            sf::CircleShape bas(radius); bas.setPosition(posX, posY + epaisseur); bas.setFillColor(cFond);
            bas.setOutlineThickness(contour); bas.setOutlineColor(cTrait); window.draw(bas);
            sf::RectangleShape corps(sf::Vector2f(radius * 2, epaisseur)); corps.setPosition(posX, posY + radius);
            corps.setFillColor(cFond); window.draw(corps);
            sf::RectangleShape bg(sf::Vector2f(contour, epaisseur)); bg.setPosition(posX - contour, posY + radius);
            bg.setFillColor(cTrait); window.draw(bg);
            sf::RectangleShape bd(sf::Vector2f(contour, epaisseur)); bd.setPosition(posX + radius*2, posY + radius);
            bd.setFillColor(cTrait); window.draw(bd);
            sf::CircleShape haut(radius); haut.setPosition(posX, posY); haut.setFillColor(cFond);
            haut.setOutlineThickness(contour); haut.setOutlineColor(cTrait); window.draw(haut);
        }
    }
}

void GameWindow::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }
}

void GameWindow::handleMouseClick(int mouseX, int mouseY) {

    // 1. SI JEU FINI -> CLIC = QUITTER
    if (isGameOver) {
        window.close();
        return;
    }

    // 2. PROTECTION TOUR
    if (currentPlayerTurn != BLACK) return;

    // 3. CLIC BOUTON (Validation)
    if (currentPath.size() >= 2 && validateButton.getGlobalBounds().contains(mouseX, mouseY)) {
        int depart = currentPath[0];
        int arrivee = currentPath.back();
        int nbPieces = currentPath.size() - 1;
        PlayerColor currentPlayer = boardState[depart].stack.back();

        if (prolog.isValidMove(boardState, depart, arrivee, nbPieces, currentPlayer)) {
            boardState = prolog.executeMove(boardState, depart, arrivee, nbPieces, currentPlayer);
            currentPath.clear();
            currentPlayerTurn = WHITE; // À l'IA !
        } else {
            std::cout << "Coup refuse par Prolog." << std::endl;
            currentPath.clear();
        }
        return;
    }

    // 4. CLIC GRILLE (Tracé)
    float size = 300.f; float margeHaut = 100.0f; float margeGauche = 100.0f;
    if (mouseX < margeGauche || mouseX > margeGauche + (3 * size) ||
        mouseY < margeHaut || mouseY > margeHaut + (3 * size)) {
        currentPath.clear(); return;
    }

    int col = (mouseX - margeGauche) / size;
    int row = (mouseY - margeHaut) / size;
    int index = row * 3 + col;

    if (currentPath.empty()) {
        if (boardState[index].stack.empty()) return;
        if (boardState[index].stack.back() == BLACK) {
            currentPath.push_back(index);
        }
    } else {
        int dernier = currentPath.back();
        if (index == dernier) {
            currentPath.pop_back(); // Undo
            return;
        }
        if (std::find(currentPath.begin(), currentPath.end(), index) != currentPath.end()) {
            currentPath.clear(); // Reset si boucle
            return;
        }
        currentPath.push_back(index);
        if (currentPath.size() > 4) currentPath.pop_back(); // Max 3 pas
    }
}