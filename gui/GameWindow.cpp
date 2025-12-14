//
// Created by v05ax on 14/12/2025.
//
#include "GameWindow.h"
#include <thread>
#include <chrono>

GameWindow::GameWindow(GameConfig config)
        : window(sf::VideoMode(1100, 1100), "Pogo - IA41"),
          p1Type(config.player1), // On récupère le choix du menu pour Noir
          p2Type(config.player2), // On récupère le choix du menu pour Blanc
          currentPlayerTurn(BLACK),
          isGameOver(false),
          winner(EMPTY)
{
    window.setFramerateLimit(60);
    initMockBoard();
    currentPath.clear();


    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "ERREUR: Impossible de charger arial.ttf" << std::endl;

    }

    // --- Configuration Bouton Valider ---
    validateButton.setSize(sf::Vector2f(200, 80));
    validateButton.setPosition(850, 1005);
    validateButton.setFillColor(sf::Color(50, 200, 50));
    validateButton.setOutlineThickness(2);
    validateButton.setOutlineColor(sf::Color::White);

    // --- Configuration Textes Fin ---
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

    // Configuration Initiale (Exemple standard ou votre test)
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
        updateGameLogic();

        window.clear(sf::Color(50, 50, 50));

        drawBoard();
        drawButton();

        if (isGameOver) {
            drawGameOver();
        }

        window.display();
    }
}

void GameWindow::updateGameLogic() {
    // 1. Vérification Victoire
    if (!isGameOver) {
        PlayerColor w = prolog.getWinner(boardState);
        if (w != EMPTY) {
            isGameOver = true;
            winner = w;

            // Setup du message de fin
            if (winner == WHITE) {
                victoryText.setString("BLANC GAGNE !");
                victoryText.setFillColor(sf::Color::White);
                victoryText.setOutlineColor(sf::Color::Black);
            } else {
                victoryText.setString("NOIR GAGNE !");
                victoryText.setFillColor(sf::Color::Black);
                victoryText.setOutlineColor(sf::Color::White);
            }

            // Centrage
            sf::FloatRect tr = victoryText.getLocalBounds();
            victoryText.setOrigin(tr.left + tr.width/2.0f, tr.top + tr.height/2.0f);
            victoryText.setPosition(550, 550);

            sf::FloatRect sr = subText.getLocalBounds();
            subText.setOrigin(sr.left + sr.width/2.0f, sr.top + sr.height/2.0f);
            subText.setPosition(550, 700);
            return;
        }
    }

    // 2. Gestion Tour IA
    if (!isGameOver) {
        // Si c'est au tour de NOIR et que NOIR est une IA
        if (currentPlayerTurn == BLACK && p1Type == TYPE_AI) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause visuelle
            playAITurn(BLACK);
        }
        // Si c'est au tour de BLANC et que BLANC est une IA
        else if (currentPlayerTurn == WHITE && p2Type == TYPE_AI) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause visuelle
            playAITurn(WHITE);
        }
    }
}

void GameWindow::playAITurn(PlayerColor color) {
    std::cout << "--- IA (" << (color == WHITE ? "Blanc" : "Noir") << ") joue ---" << std::endl;

    MoveInfo move = prolog.getAIMove(boardState, color);

    if (move.start != -1) {
        boardState = prolog.executeMove(boardState, move.start, move.end, move.numPieces, color);
        std::cout << "-> Deplacement IA effectue." << std::endl;
        currentPlayerTurn = (color == WHITE) ? BLACK : WHITE; // Changement de tour
    } else {
        std::cout << "-> IA bloquee (Aucun coup)." << std::endl;
        currentPlayerTurn = (color == WHITE) ? BLACK : WHITE;
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
    // Si la partie est finie, un clic ferme la fenêtre
    if (isGameOver) {
        window.close();
        return;
    }

    if (currentPlayerTurn == BLACK && p1Type == TYPE_AI) return;
    if (currentPlayerTurn == WHITE && p2Type == TYPE_AI) return;

    // --- GESTION DU BOUTON VALIDER ---
    if (currentPath.size() >= 2 && validateButton.getGlobalBounds().contains(mouseX, mouseY)) {
        int depart = currentPath[0];
        int arrivee = currentPath.back();
        int nbPieces = currentPath.size() - 1;

        if (boardState[depart].stack.empty()) return;

        //On vérifie le propriétaire au SOMMET (index 0)
        PlayerColor owner = boardState[depart].stack.front();

        // On appelle Prolog
        if (prolog.isValidMove(boardState, depart, arrivee, nbPieces, owner)) {
            boardState = prolog.executeMove(boardState, depart, arrivee, nbPieces, owner);
            currentPath.clear();
            currentPlayerTurn = (currentPlayerTurn == WHITE) ? BLACK : WHITE;
        } else {
            std::cout << "Coup invalide !" << std::endl;
            currentPath.clear();
        }
        return;
    }

    // --- GESTION DES CLICS SUR LA GRILLE ---
    float size = 300.f; float margeHaut = 100.0f; float margeGauche = 100.0f;

    // Si clic hors plateau
    if (mouseX < margeGauche || mouseX > margeGauche + (3 * size) ||
        mouseY < margeHaut || mouseY > margeHaut + (3 * size)) {
        currentPath.clear();
        return;
    }

    int col = (mouseX - margeGauche) / size;
    int row = (mouseY - margeHaut) / size;
    int index = row * 3 + col;

    // Logique de sélection de chemin
    if (currentPath.empty()) {
        // Sélection départ
        if (boardState[index].stack.empty()) return;

        // On ne peut sélectionner que NOS pions
        if (boardState[index].stack.front() == currentPlayerTurn) {
            currentPath.push_back(index);
        }
    } else {
        // Continuation du chemin
        int dernier = currentPath.back();

        // Clic sur la dernière case = Annuler le dernier pas
        if (index == dernier) {
            currentPath.pop_back();
            return;
        }

        // Clic sur une case déjà visitée
        if (std::find(currentPath.begin(), currentPath.end(), index) != currentPath.end()) {
            currentPath.clear();
            return;
        }

        // Ajout au chemin (max 3 pas + départ = 4 cases)
        currentPath.push_back(index);
        if (currentPath.size() > 4) currentPath.pop_back();
    }
}

void GameWindow::drawBoard() {
    float size = 300.f; float margeHaut = 100.0f; float margeGauche = 100.0f;
    float radius = 70.f; float epaisseur = 20.0f; float contour = 3.0f;

    for (int i = 0; i < 9; i++) {
        float x = (i % 3) * size + margeGauche;
        float y = (i / 3) * size + margeHaut;

        // Dessin de la case
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(100, 100, 100));

        // Gestion des bordures
        auto it = std::find(currentPath.begin(), currentPath.end(), i);
        if (it != currentPath.end()) {
            if (it == currentPath.begin()) {
                rect.setOutlineThickness(-10.0f); rect.setOutlineColor(sf::Color::Green); // Départ
            } else {
                rect.setOutlineThickness(-10.0f); rect.setOutlineColor(sf::Color::Cyan);  // Chemin
            }
        } else {
            rect.setOutlineThickness(-5.0f); rect.setOutlineColor(sf::Color::Black); // Normal
        }
        window.draw(rect);

        // Pour afficher correctement, on doit dessiner
        // d'abord le BAS (index max), puis remonter vers le HAUT (index 0).

        std::vector<PlayerColor>& pile = boardState[i].stack;
        if (!pile.empty()) {
            float groundY = y + size/2 + 50; // Position Y du bas de la pile visuelle

            // On boucle de la FIN (Bas) vers le DÉBUT (Haut)
            for (int k = pile.size() - 1; k >= 0; k--) {

                // La hauteur visuelle dépend de combien de pions sont en-dessous
                // Pions en dessous = (pile.size() - 1) - k
                int heightLevel = (pile.size() - 1) - k;

                float posX = x + size/2 - radius;
                float posY = groundY - radius - (heightLevel * epaisseur);

                sf::Color cFond = (pile[k] == WHITE) ? sf::Color::White : sf::Color::Black;
                sf::Color cTrait = (pile[k] == WHITE) ? sf::Color::Black : sf::Color::White;

                // Forme du pion (Cylindre 3D simplifié)
                sf::CircleShape bas(radius);
                bas.setPosition(posX, posY + epaisseur);
                bas.setFillColor(cFond);
                bas.setOutlineThickness(contour);
                bas.setOutlineColor(cTrait);
                window.draw(bas);

                sf::RectangleShape corps(sf::Vector2f(radius * 2, epaisseur));
                corps.setPosition(posX, posY + radius);
                corps.setFillColor(cFond);
                window.draw(corps);

                // Traits latéraux pour le style
                sf::RectangleShape bg(sf::Vector2f(contour, epaisseur));
                bg.setPosition(posX - contour, posY + radius); bg.setFillColor(cTrait); window.draw(bg);
                sf::RectangleShape bd(sf::Vector2f(contour, epaisseur));
                bd.setPosition(posX + radius*2, posY + radius); bd.setFillColor(cTrait); window.draw(bd);

                sf::CircleShape haut(radius);
                haut.setPosition(posX, posY);
                haut.setFillColor(cFond);
                haut.setOutlineThickness(contour);
                haut.setOutlineColor(cTrait);
                window.draw(haut);
            }
        }
    }
}

void GameWindow::drawButton() {
    // Bouton visible seulement si un coup est prêt et le jeu en cours
    if (currentPath.size() >= 2 && !isGameOver) {
        window.draw(validateButton);

        sf::Text txt("VALIDER", font, 30);
        sf::FloatRect b = txt.getLocalBounds();
        txt.setOrigin(b.left + b.width/2.0f, b.top + b.height/2.0f);
        txt.setPosition(validateButton.getPosition().x + 100, validateButton.getPosition().y + 40);
        txt.setFillColor(sf::Color::White);
        window.draw(txt);
    }
}

void GameWindow::drawGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(1100, 1100));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    window.draw(victoryText);
    window.draw(subText);
}