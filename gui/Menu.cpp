//
// Created by v05ax on 14/12/2025.
//

#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// Fonction utilitaire pour dessiner un bouton
void drawButton(sf::RenderWindow& window, sf::Font& font, const std::string& text,
                float x, float y, bool isSelected) {
    sf::RectangleShape btn(sf::Vector2f(200, 50));
    btn.setPosition(x, y);
    btn.setFillColor(isSelected ? sf::Color(50, 200, 50) : sf::Color(100, 100, 100));
    btn.setOutlineThickness(2);
    btn.setOutlineColor(sf::Color::White);

    sf::Text txt(text, font, 20);
    sf::FloatRect bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.left + bounds.width/2.0f, bounds.top + bounds.height/2.0f);
    txt.setPosition(x + 100, y + 25);
    txt.setFillColor(sf::Color::White);

    window.draw(btn);
    window.draw(txt);
}

GameConfig runMenu() {
    sf::RenderWindow window(sf::VideoMode(600, 500), "Configuration Pogo");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Erreur police" << std::endl;
    }

    GameConfig config = {TYPE_HUMAN, TYPE_AI}; // Par défaut

    sf::RectangleShape startBtn(sf::Vector2f(200, 60));
    startBtn.setPosition(200, 350);
    startBtn.setFillColor(sf::Color(50, 50, 200));

    sf::Text title("CONFIGURATION", font, 40); title.setPosition(130, 30);
    sf::Text p1Txt("Joueur 1 (Noir):", font, 20); p1Txt.setPosition(50, 120);
    sf::Text p2Txt("Joueur 2 (Blanc):", font, 20); p2Txt.setPosition(50, 220);
    sf::Text startTxt("LANCER", font, 25); startTxt.setPosition(250, 365);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); exit(0); }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x;
                int my = event.mouseButton.y;

                // Choix Joueur 1
                if (my >= 150 && my <= 200) {
                    if (mx >= 50 && mx <= 250) config.player1 = TYPE_HUMAN;
                    if (mx >= 300 && mx <= 500) config.player1 = TYPE_AI;
                }
                // Choix Joueur 2
                if (my >= 250 && my <= 300) {
                    if (mx >= 50 && mx <= 250) config.player2 = TYPE_HUMAN;
                    if (mx >= 300 && mx <= 500) config.player2 = TYPE_AI;
                }
                // Bouton Start
                if (startBtn.getGlobalBounds().contains(mx, my)) {
                    window.close();
                    return config;
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(title); window.draw(p1Txt); window.draw(p2Txt);
        drawButton(window, font, "Humain", 50, 150, config.player1 == TYPE_HUMAN);
        drawButton(window, font, "IA", 300, 150, config.player1 == TYPE_AI);
        drawButton(window, font, "Humain", 50, 250, config.player2 == TYPE_HUMAN);
        drawButton(window, font, "IA", 300, 250, config.player2 == TYPE_AI);
        window.draw(startBtn); window.draw(startTxt);
        window.display();
    }
    return config;
}