#pragma once
#include <SFML/Graphics.hpp>

struct UIElements {
    sf::RectangleShape pauseButton, playButton, restartButton;
    sf::Text pauseText, playText, restartText;
};

UIElements initUI(sf::Font& font);
void drawUI(sf::RenderWindow& window, UIElements& ui);
