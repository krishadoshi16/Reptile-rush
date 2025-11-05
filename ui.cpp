#include "ui.hpp"

UIElements initUI(sf::Font& font) {
    UIElements ui;

    ui.pauseButton = sf::RectangleShape(sf::Vector2f(80, 30));
    ui.playButton = sf::RectangleShape(sf::Vector2f(80, 30));
    ui.restartButton = sf::RectangleShape(sf::Vector2f(80, 30));

    ui.pauseButton.setFillColor(sf::Color::Yellow);
    ui.playButton.setFillColor(sf::Color::Blue);
    ui.restartButton.setFillColor(sf::Color::Green);

    ui.pauseText = sf::Text("Pause", font, 16);
    ui.playText = sf::Text("Play", font, 16);
    ui.restartText = sf::Text("Restart", font, 16);

    ui.pauseText.setFillColor(sf::Color::Black);
    ui.playText.setFillColor(sf::Color::White);
    ui.restartText.setFillColor(sf::Color::Black);

    ui.pauseButton.setPosition(330, 10);
    ui.playButton.setPosition(420, 10);
    ui.restartButton.setPosition(510, 10);
    ui.pauseText.setPosition(340, 15);
    ui.playText.setPosition(435, 15);
    ui.restartText.setPosition(515, 15);

    return ui;
}

void drawUI(sf::RenderWindow& window, UIElements& ui) {
    window.draw(ui.pauseButton);
    window.draw(ui.playButton);
    window.draw(ui.restartButton);
    window.draw(ui.pauseText);
    window.draw(ui.playText);
    window.draw(ui.restartText);
}
