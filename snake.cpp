#include "snake.hpp"

void initSnake(std::vector<sf::RectangleShape>& snake, int gridSize, int blockSize) {
    snake.clear();
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape seg(sf::Vector2f(blockSize, blockSize));
        seg.setFillColor(sf::Color(0, 180 - i * 40, 0));
        seg.setPosition(blockSize * (gridSize / 2 - i), blockSize * (gridSize / 2));
        snake.push_back(seg);
    }
}

void moveSnake(std::vector<sf::RectangleShape>& snake, int blockSize, int dir) {
    for (int i = snake.size() - 1; i > 0; i--)
        snake[i].setPosition(snake[i - 1].getPosition());
}

void drawSnake(sf::RenderWindow& window, std::vector<sf::RectangleShape>& snake) {
    for (size_t i = 0; i < snake.size(); i++) {
        int greenValue = 180 - (int)i * 30;
        if (greenValue < 50) greenValue = 50;
        snake[i].setFillColor(sf::Color(0, greenValue, 0));
        window.draw(snake[i]);
    }
}

bool checkSelfCollision(const std::vector<sf::RectangleShape>& snake) {
    for (size_t i = 1; i < snake.size(); i++) {
        if (snake[0].getPosition() == snake[i].getPosition())
            return true;
    }
    return false;
}